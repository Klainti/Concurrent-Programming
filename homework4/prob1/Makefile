CC = gcc
CFLAGS = -Wall -DDEBUG -DDEBUG_COLOR 
OBJ = check.o command.o
LDFLAGS = -lpthread
DEPS = debug.h

check: $(OBJ)
	$(CC) $(OBJ) -o $@  $(LDFLAGS)

%.o : %.c $(DEPS)
		$(CC) $(CFLAGS) -c $< $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf *.o *.dSYM
