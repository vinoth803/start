CC=gcc
LFLAGS=-lpthread
DEBUG=-g
TARGET=game

all:
	$(CC) $(LFLAGS) $(DEBUG) battledotnet.c threads.c log.c -o $(TARGET)

clean:
	rm $(TARGET)

