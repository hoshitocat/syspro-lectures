CFLAGS = -g -DDEBUG

all: redirect server

redirect: redirect.o netlib.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o redirect redirect.o netlib.o

server: server.o netlib.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o server server.o netlib.o

redirect.o netlib.o server.o: netlib.h

clean:
	$(RM) redirect server redirect.o netlib.o server.o
