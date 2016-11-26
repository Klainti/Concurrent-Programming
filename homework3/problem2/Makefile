CC = gcc
CFLAGS = -Wall -DDEBUG -DDEBUG_COLOR 
OBJ = prob2.o
LDFLAGS = -lpthread
DEPS = debug.h

prob2: $(OBJ) 
	$(CC) $@.o -o $@ $(LDFLAGS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -rf *.o *.dSYM
