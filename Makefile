HEADERS_DIR = headers
SOURCES_DIR = sources
BINARIES_DIR = bin
VALGRIND_TEST_CONTEXT = tests/valgrind-test
COPY_BIN_TO_VALGRIND_CONTEXT = cp -v  morpion_* $(VALGRIND_TEST_CONTEXT)/morpion; cp -Rv assets $(VALGRIND_TEST_CONTEXT)
CC = gcc -Wall -Wextra -pedantic

all : morpion

morpion : morpion_gtk morpion_console

morpion_console : $(BINARIES_DIR)/interface_console.o $(BINARIES_DIR)/history.o $(BINARIES_DIR)/board.o $(BINARIES_DIR)/utils.o $(BINARIES_DIR)/main.o
	$(CC) $^ -o $@
	$(COPY_BIN_TO_VALGRIND_CONTEXT)

morpion_gtk : $(BINARIES_DIR)/interface_gtk.o $(BINARIES_DIR)/history.o $(BINARIES_DIR)/board.o $(BINARIES_DIR)/utils.o $(BINARIES_DIR)/main.o
	$(CC) $^ -o $@
	$(COPY_BIN_TO_VALGRIND_CONTEXT)

$(BINARIES_DIR)/interface_console.o : $(SOURCES_DIR)/interface_console.c $(HEADERS_DIR)/interface.h $(HEADERS_DIR)/history.h $(HEADERS_DIR)/utils.h
	$(CC) -c $< -o $@

$(BINARIES_DIR)/interface_gtk.o : $(SOURCES_DIR)/interface_gtk.c $(HEADERS_DIR)/interface.h $(HEADERS_DIR)/history.h $(HEADERS_DIR)/utils.h
	$(CC) -c $< -o $@

$(BINARIES_DIR)/history.o : $(SOURCES_DIR)/history.c $(HEADERS_DIR)/board.h $(HEADERS_DIR)/history.h $(HEADERS_DIR)/interface.h
	$(CC) -c $< -o $@

$(BINARIES_DIR)/board.o : $(SOURCES_DIR)/board.c $(HEADERS_DIR)/board.h $(HEADERS_DIR)/history.h $(HEADERS_DIR)/interface.h $(HEADERS_DIR)/utils.h
	$(CC) -c $< -o $@

$(BINARIES_DIR)/utils.o : $(SOURCES_DIR)/utils.c $(HEADERS_DIR)/utils.h 
	$(CC) -c $< -o $@

$(BINARIES_DIR)/main.o : $(SOURCES_DIR)/main.c $(HEADERS_DIR)/board.h $(HEADERS_DIR)/history.h $(HEADERS_DIR)/interface.h $(HEADERS_DIR)/utils.h
	$(CC) -c $< -o $@

$(shell mkdir -p $(BINARIES_DIR))
