cc -c logutil.c
cc -c main.c
cc -o app -pthread main.o logutil.o
