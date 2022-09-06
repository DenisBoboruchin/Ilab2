CC		= g++
CFLAGS  = -c -w -Wall

all: main.o cashe.o
	$(CC) main.cpp src/cashe.cpp -o main 

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

cashe.o: src/cashe.cpp
	$(CC) $(CFLAGS) src/cashe.cpp

clean:
	rm -rf *.out *.o main
