# Assignment 5 - Hamming Codes

---

Programs for generating and decoding (8, 4) systematic Hamming codes

---

## Build Targets

* `make all`

Builds `encode`, `decode`, `error`, and `entropy`.

* `make encode`

Builds the `encode` program

* `make decode`

Builds the `decode` program

* `make error`

Builds the `error` program

* `make entropy`

Builds the `entropy` program

* `make clean`

Removes object and binary files

* `make format`

Formats the source files using clang-format

* `make scan-build`

Analyzes the program for bugs

---

## Running

`./encode [-h] [-i infile] [-o outfile]`

`./decode [-h] [-v] [-i infile] [-o outfile]`

---

## Command Line arguments:

For the encode program:

- `-h`: Program usage and help.
- `-i infile`: Input data to encode (default: stdin).
- `-o outfile`: Output of encoded data (default: stdout).

For the decode program:

- `-h`: Program usage and help.
- `-v`: Print statistics of decoding to stderr.
- `-i infile`: Input data to encode (default: stdin).
- `-o outfile`: Output of encoded data (default: stdout).

## Bugs

Running scan-build warns of a potential memory leak from `infile_name` and `outfile_name`,
but these strings are freed and their pointers are set to `NULL` when they are no longer needed
and every time the program prematurely exits on error.


