CFLAGS = -std=c99 -Wall -pedantic -g
LDFLAGS = -ljansson -lcurl

OBJECTS = account.o \
	  request.o \
	  droplet.o \
	  sea.o

sea: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $@

all: sea

clean:
	$(RM) *.o
	$(RM) sea

.PHONY: all clean
