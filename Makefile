all: clean lzw rc4
lzw:
	gcc test_lzw.c lzw.c -o lzw
rc4:
	gcc test_rc4.c rc4.c -o rc4
clean:
	rm -f lzw rc4