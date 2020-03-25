all: clean crc16 lzw rc4
crc16:
	gcc test_crc16.c crc16.c -o crc16
lzw:
	gcc test_lzw.c lzw.c -o lzw
rc4:
	gcc test_rc4.c rc4.c -o rc4
clean:
	rm -f crc16 lzw rc4