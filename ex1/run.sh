#!/bin/sh

cc -c alloc2.c
cc -c main2.c
cc -o ex1 main2.o alloc2.o
./ex1
