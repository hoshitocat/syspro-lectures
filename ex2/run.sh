#! /bin/sh

cc -c alloc3.c
cc -c main3.c
cc -o ex2 alloc3.o main3.o
./ex2
