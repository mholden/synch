CC=gcc
CFLAGS=-g
DEVEL=/home/mholden/devel
SN=$(DEVEL)/synch
INCLUDES=-I$(SN)/include

all: test_synch

test_synch: test_synch.c ../synch.c ../include/synch.h
	$(CC) $(CFLAGS) $(INCLUDES) test_synch.c ../synch.c -pthread -o test_synch

clean:
	rm *o test_synch

.PHONY: all clean
