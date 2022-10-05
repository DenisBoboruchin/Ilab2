CC = g++
CFLAGS = -c -w -Wall -Wextra

all: main.o dir_dump
	$(CC) main.o -o main 

main.o: main.cpp include/cache.h
	$(CC) $(CFLAGS) main.cpp include/cache.h

dir_dump:
	@if [ ! -d "dump" ] ; then mkdir "dump" ; fi

clean:
	rm -rf *.out *.o main dump/*
