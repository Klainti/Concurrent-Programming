CC = gcc
CFLAGS = -Wall -DDEBUG -DDEBUG_COLOR 
OBJ = prob3.o libcsp.o
LDFLAGS = -lpthread
DEPS = debug.h

prob3: $(OBJ)
	$(CC) $(OBJ) -o $@  $(LDFLAGS)

%.o : %.c $(DEPS)
		$(CC) $(CFLAGS) -c $< $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf *.o *.dSYM
