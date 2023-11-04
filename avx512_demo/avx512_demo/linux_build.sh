#! /bin/sh
gcc main.c -o main.c.o -c -std=gnu17
gcc test.S -o test.S.o -c
gcc main.c.o test.S.o -o test
rm main.c.o test.S.o

