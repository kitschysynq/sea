CC=gcc
CFLAGS=-Wall
LDFLAGS=-ljansson -lcurl

OBJECTS=sea.o request.o

sea: $(OBJECTS)
	    $(CC) $(CFLAGS) $(OBJECTS) -o sea $(LDFLAGS)

all:sea

.PHONY: clean
	clean:
	    rm -f *~ *.o client


# all:
# 	gcc -I . -o sea -ljansson drosh.c account.c
