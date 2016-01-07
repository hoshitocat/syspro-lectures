cc -c logutil.c
cc -c server.c
cc -o app -pthread server.o logutil.o
