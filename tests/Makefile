BINARIES_DIR = bin

CC = gcc -Wall -pedantic

all : test

test : test.c ../$(BINARIES_DIR)/interface.o ../$(BINARIES_DIR)/history.o ../$(BINARIES_DIR)/board.o ../$(BINARIES_DIR)/utils.o
	$(CC) $^ -o $@ -lcunit
