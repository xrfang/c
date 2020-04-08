all: clean crc16 lzw rc4 split packager
crc16:
	gcc test_crc16.c crc16.c -o crc16
lzw:
	gcc test_lzw.c lzw.c -o lzw
	gcc -pthread -o lzw_pipe test_lzw_pipe.c lzw_pipe.c lzw.c
rc4:
	gcc test_rc4.c rc4.c -o rc4
split:
	gcc -pthread -o split test_split.c fsplitter.c
packager:
	gcc -pthread -o packager test_packager.c packager.c fsplitter.c lzw_pipe.c lzw.c
clean:
	rm -f crc16 lzw lzw_pipe packager rc4 split