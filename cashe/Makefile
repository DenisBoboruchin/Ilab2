CC		= g++
CFLAGS  = -c -w -Wall -Wextra

all: main.o cashe.o dir_dump
	$(CC) main.cpp src/cashe.cpp -o main 

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

cashe.o: src/cashe.cpp
	$(CC) $(CFLAGS) src/cashe.cpp

dir_dump:
	@if [ ! -d "dump" ] ; then mkdir "dump" ; fi

clean:
	rm -rf *.out *.o main dump/*
