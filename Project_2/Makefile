CC=gcc
CFLAGS= -Wall -pedantic -std=c99 -g -D_POSIX_C_SOURCE
LDFLAGS= -lpthread
LIB=ar rcs

all: main transmitter tester

main: main.c libloglib.a log.h
	@echo "Creating B program..."
	$(CC) $(CFLAGS) -o main main.c -L. -lloglib $(LDFLAGS)

transmitter: transmitter.c
	@echo "Creating A program..."
	$(CC) $(CFLAGS) -o transmitter transmitter.c -L.

tester: tester.c
	@echo "Creating tester program..."
	$(CC) $(CFLAGS) -o tester tester.c


log.o: log.c
	@echo "Creating library executable..."
	$(CC)	-c log.c -o log.o

libloglib.a: log.o
	$(LIB) libloglib.a log.o && rm -f log.o

clean:
	@echo "Cleaning up..."
	rm -f main transmitter tester fifopipe textpipe idpipe *.o *.a
