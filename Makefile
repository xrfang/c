all: clean crc16 lzw map packager rc4 split
clean:
	rm -f crc16 lzw lzw_pipe map packager rc4 split
crc16:
	gcc test_crc16.c crc16.c -o crc16
lzw:
	gcc test_lzw.c lzw.c -o lzw
	gcc -pthread -o lzw_pipe test_lzw_pipe.c lzw_pipe.c lzw.c
map:
	gcc test_map.c map.c -o map
packager:
	gcc -pthread -o packager test_packager.c packager.c fsplitter.c lzw_pipe.c lzw.c
rc4:
	gcc test_rc4.c rc4.c -o rc4
split:
	gcc -pthread -o split test_split.c fsplitter.c