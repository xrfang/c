/*
 * LZW compression
 * https://codereview.stackexchange.com/questions/98498
 *
 * - Uses fixed length 12-bit encodings.
 * - Outputs in MSB format.
 * - When encoding table fills up, then table is reset back to the initial
 *   256 entries.
 * - Written in C89 style.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DICT_BITS 12
#define DICT_MAX (1 << DICT_BITS)
#define END_OF_FILE_CODE 0xffff

/* The LZW encoder builds a trie out of the input file, but only adds one
 * new trie node per sequence that it outputs.  There will be a maximum
 * of DICT_MAX sequences, so the child trie pointers can be uint16_t values
 * which are the node indices of the child nodes.  An index of 0 is like
 * a NULL pointer, because no node can point to node 0. */
typedef struct DictNode
{
    uint16_t child[256];
} DictNode;

/* Each dictionary entry is a byte sequence and a length. */
typedef struct DictEntry
{
    uint8_t *seq;
    int len;
} DictEntry;

/* We use a custom allocator because the maximum length of all sequences is
 * predictable, and we can keep all the sequences packed together by using
 * one big allocation and carving it up. */
typedef struct AllocInfo
{
    uint8_t *base;
    int len;
    uint8_t *nextAlloc;
} AllocInfo;

/* Use a struct to hold input state so we can read 12-bit codes from the
 * input file. */
typedef struct InputState
{
    FILE *fp;
    int leftoverBits;
    int leftoverCode;
} InputState;

// Intializes the custom allocator.
static void AllocInit(AllocInfo *alloc, int size)
{
    alloc->base = malloc(size);
    alloc->len = size;
    alloc->nextAlloc = alloc->base;
}

// Allocate memory using custom allocator.
static uint8_t *Allocate(AllocInfo *alloc, int len)
{
    uint8_t *ret = alloc->nextAlloc;
    // Round up to the nearest 4 byte alignment.
    len = (len + 3) & ~3;
    alloc->nextAlloc += len;
    return ret;
}

// Reads a 12 bit code from the file in a MSB manner.
static int ReadNextCode(InputState *inState)
{
    int code;
    int b0 = fgetc(inState->fp);
    if (b0 == EOF)
        return END_OF_FILE_CODE;
    if (inState->leftoverBits == 0)
    {
        int b1 = fgetc(inState->fp);
        if (b1 == EOF)
            return END_OF_FILE_CODE;
        code = (b0 << 4) | (b1 >> 4);
        inState->leftoverBits = 4;
        inState->leftoverCode = (b1 & 0xf) << 8;
    }
    else
    {
        code = inState->leftoverCode | b0;
        inState->leftoverBits = 0;
    }
    return code;
}

int lzw_compress(const char *inFile, const char *outFile)
{
    FILE *in = NULL;
    FILE *out = NULL;
    DictNode *dict = NULL;
    int res = 0;

    in = fopen(inFile, "rb");
    if (in == NULL)
    {
        res = 1;
        goto done;
    }
    out = fopen(outFile, "wb");
    if (out == NULL)
    {
        res = 2;
        goto done;
    }
    int dictSize = 256;
    int nextByte = fgetc(in);
    uint16_t curNode = nextByte;
    int leftoverBits = 0;
    int leftoverByte = 0;
    // Abort on empty input file.
    if (nextByte == EOF)
    {
        res = 1;
        goto done;
    }
    // Initialize the dictionary.
    dict = calloc(DICT_MAX, sizeof(DictNode));
    if (dict == NULL)
    {
        res = 3;
        goto done;
    }
    while (1)
    {
        int curByte = fgetc(in);
        // Check if the file ended.  If so, output the last code and any
        // leftover bits, and then break out of the main loop.
        if (curByte == EOF)
        {
            if (leftoverBits == 0)
            {
                fputc(curNode >> 4, out);
                fputc(curNode << 4, out);
            }
            else
            {
                fputc(leftoverByte | (curNode >> 8), out);
                fputc(curNode, out);
            }
            break;
        }
        // Follow the new byte down the trie.
        uint16_t nextNode = dict[curNode].child[curByte];
        if (nextNode != 0)
        {
            // The sequence exists, keep searching down the trie.
            curNode = nextNode;
            continue;
        }
        // The sequence doesn't exist.  First, output the code for curNode.
        // This is hardcoded for 12-bit output codes.
        if (leftoverBits == 0)
        {
            fputc(curNode >> 4, out);
            leftoverBits = 4;
            leftoverByte = (curNode << 4);
        }
        else
        {
            fputc(leftoverByte | (curNode >> 8), out);
            fputc(curNode, out);
            leftoverBits = 0;
        }
        // Now, extend the sequence in the trie by the new byte.
        if (dictSize < DICT_MAX)
            dict[curNode].child[curByte] = dictSize++;
        else
        {
            // The trie hit max size.  Instead of extending the trie,
            // clear it back to the original 256 entries.
            memset(dict, 0, DICT_MAX * sizeof(dict[0]));
            dictSize = 256;
        }
        // Start over a new sequence with the current byte.
        curNode = curByte;
    }
done:
    if (dict != NULL)
        free(dict);
    if (in != NULL)
        fclose(in);
    if (out != NULL)
        fclose(out);
    return res;
}

int lzw_decompress(const char *inFile, const char *outFile)
{
    FILE *in = NULL;
    FILE *out = NULL;
    DictEntry *dict = NULL;
    AllocInfo allocInfo;
    memset(&allocInfo, 0, sizeof(allocInfo));
    int res = 0;

    in = fopen(inFile, "rb");
    if (in == NULL)
    {
        res = 1;
        goto done;
    }
    out = fopen(outFile, "wb");
    if (out == NULL)
    {
        res = 2;
        goto done;
    }
    int dictSize = 256;
    InputState inState = {in, 0, 0};
    uint16_t prevCode = ReadNextCode(&inState);
    uint8_t *mark = NULL;
    int i = 0;
    // Abort on empty input file.
    if (prevCode == END_OF_FILE_CODE)
    {
        res = 1;
        goto done;
    }
    // The maximum of all sequences will be if the sequences increase in length
    // steadily from 1..DICT_MAX.  Add in an extra 2 bytes per entry to account
    // for the fact that we round each allocation to 4 bytes in size.
    AllocInit(&allocInfo, DICT_MAX * DICT_MAX / 2 + DICT_MAX * 2);
    if (allocInfo.base == NULL)
    {
        res = 3;
        goto done;
    }
    // Initialize dictionary to single character entries.
    dict = calloc(DICT_MAX, sizeof(DictEntry));
    if (dict == NULL)
    {
        res = 3;
        goto done;
    }
    for (i = 0; i < dictSize; i++)
    {
        dict[i].seq = Allocate(&allocInfo, 1);
        dict[i].seq[0] = i;
        dict[i].len = 1;
    }
    // This mark is used to indicate where we should reset the allocations
    // to when we reset the dictionary to 256 entries.
    mark = allocInfo.nextAlloc;
    // Output the first code sequence, which is always a single byte.
    fputc(prevCode, out);
    while (1)
    {
        uint16_t code = ReadNextCode(&inState);
        if (code > dictSize)
        {
            // The normal case would be that the file ended.
            if (code != END_OF_FILE_CODE)
            { // Otherwise there was a problem with the input file.
                res = 4;
            }
            break;
        }
        // Add entry to dictionary first.  That way, if we need to use
        // the just added dictionary entry, it will be ready to use.
        if (dictSize == DICT_MAX)
        {
            // Dictionary hit max size.  Reset it.
            dictSize = 256;
            allocInfo.nextAlloc = mark;
        }
        else
        {
            // Extend dictionary by one entry.  The new entry is the same
            // as the previous entry plus one character.
            int prevLen = dict[prevCode].len;
            dict[dictSize].len = prevLen + 1;
            dict[dictSize].seq = Allocate(&allocInfo, prevLen + 1);
            memcpy(dict[dictSize].seq, dict[prevCode].seq, prevLen);
            // The last character normally comes from the first character
            // of the current code.  However, if it is the newly added entry,
            // then it is the first character of the previous code.
            if (code == dictSize)
                dict[dictSize++].seq[prevLen] = dict[prevCode].seq[0];
            else
                dict[dictSize++].seq[prevLen] = dict[code].seq[0];
        }
        // Output code sequence to file.
        fwrite(dict[code].seq, 1, dict[code].len, out);
        prevCode = code;
    }
done:
    if (dict != NULL)
        free(dict);
    if (allocInfo.base != NULL)
        free(allocInfo.base);
    if (in != NULL)
        fclose(in);
    if (out != NULL)
        fclose(out);
    return res;
}
