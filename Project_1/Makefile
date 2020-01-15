CC=gcc
CFLAGS= -Wall -pedantic -std=c99 -g -D_POSIX_C_SOURCE
LIB=ar rcs

all: main new_world

main: main.c hellolib.h log.h
	@echo "Creating the main program..."
	$(CC) $(CFLAGS) -o main main.c log.c

new_world: new_world.c libhellolib.a hellolib.h log.h
	@echo "Creating program for the children..."
	$(CC) $(CFLAGS) -o new_world new_world.c log.c -L. -lhellolib

hellolib.o: hellolib.c
	@echo "Creating library executable..."
	$(CC)	-c hellolib.c -o hellolib.o

libhellolib.a: hellolib.o
	$(LIB) libhellolib.a hellolib.o && rm -f hellolib.o

clean:
	@echo "Cleaning up..."
	rm -f main new_world *.o *.a
