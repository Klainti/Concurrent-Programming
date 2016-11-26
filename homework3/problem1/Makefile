CC = gcc
CFLAGS = -Wall -DDEBUG -DDEBUG_COLOR 
OBJ = prob1.o
LDFLAGS = -lpthread
DEPS = debug.h

prob1: $(OBJ) 
	$(CC) $@.o -o $@ $(LDFLAGS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -rf *.o *.dSYM
