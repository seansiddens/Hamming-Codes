CC      = clang
CFLAGS  = -Wall -Wpedantic -Wextra -Werror
LFLAGS  = -lm

.PHONY: all clean format

all: encode decode error entropy

encode: encode.c bv.c bm.c hamming.c util.c
	$(CC) encode.c bv.c bm.c hamming.c util.c $(CFLAGS) -o encode

decode: decode.c bv.c bm.c hamming.c util.c
	$(CC) decode.c bv.c bm.c hamming.c util.c $(CFLAGS) -o decode

error: error.c
	$(CC) error.c $(CFLAGS) -o error

entropy: entropy.c
	$(CC) entropy.c $(CFLAGS) $(LFLAGS) -o entropy

format:
	clang-format -i -style=file *.[ch]

clean:
	rm -rf encode decode error entropy

scan-build: clean
	scan-build make
