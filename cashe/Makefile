CC		= g++
CFLAGS  = -c -w -Wall -Wextra

all: main.o cashe.o dir_dump
	$(CC) main.o -o main 

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

cashe.o: include/cashe.h
	$(CC) $(CFLAGS) include/cashe.h

dir_dump:
	@if [ ! -d "dump" ] ; then mkdir "dump" ; fi

clean:
	rm -rf *.out *.o main dump/*
