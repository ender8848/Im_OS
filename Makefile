CC=g++ -Wall

all: main

main: helper.o main.o
	$(CC) -pthread -o main helper.o main.o

main.o: helper.c main.c
	$(CC) -c helper.c main.c

tidy:
	rm -f *.o core

clean:
	rm -f main producer consumer *.o core
