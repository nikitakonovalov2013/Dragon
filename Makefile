CC=gcc
CFLAGS=-Wall

SOURCES=dragon.c dragon.h

all:
	$(CC) $(CFALGS) $(SOURCES) -o dragon 
otl:
	$(CC) -g $(SOURCES) -o dragontest
