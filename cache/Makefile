INC_DIR = include

CC = clang++
CFLAGS = -c -g -w -Wall -Wextra -I$(INC_DIR)

all: main.o cache dir_dump
	$(CC) main.o -o main 

main.o: main.cpp cache.gch
	$(CC) $(CFLAGS) main.cpp

cache.gch: include/cache.h
	$(CC) $(CFLAGS) include/cache.h

cache: cache.o
	$(CC) cache.o -o cache

cache.o: src/cache.cpp cache.gch
	$(CC) $(CFLAGS) src/cache.cpp

dir_dump:
	@if [ ! -d "dump" ] ; then mkdir "dump" ; fi

clean:
	rm -rf *.out *.o *.gch main cache dump/*
