# Compiler
CC = gcc
LDFLAGS = -o $(BIN) $(LIBPATH) $(LIBS)
CFDEBUG = $(CFLAGS) -g -DDEBUG $(LDFLAGS)
RM = /bin/rm -f

SRC = src/serialize.c
HDR = src/headers/serialize.h

BIN = serlibc
BINS = serlibc.so
BUILD_DIR = bin
LIB_DIR = lib
CFLAGS = -std=c18 -Wall

# All .c source files
SRC = src/serialize.c

all: $(BINS)

serlibc.so: $(SRC) $(HDR)
	$(CC) $(CFLAGS) -fPIC -shared -o $(LIB_DIR)/$@ $(SRC) -lc

# prevent confusion with any files named "clean"
.PHONY: clean
clean:
	$(RM) *.o *.so *~ $(BUILD_DIR)/$(BIN)

depend: $(SRC)
	makedepend $(INCLUDES) $^

debug_code:
	$(RM) debug/debug
	$(CC) -g -o debug/debug $(SRC) $(CFLAGS) $(INCLUDES) $(LIBS)



