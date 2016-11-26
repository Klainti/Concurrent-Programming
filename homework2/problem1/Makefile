CC = gcc
CFLAGS = -Wall 
OBJ = mandelCore.o mandelGUI.o
LDFLAGS = -L /usr/X11R6/lib -lX11 -lpthread
DEPS = mandelCore.h

mandelGUI: $(OBJ) 
	$(CC) $< $@.o -o $@ $(LDFLAGS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -rf *.o *.dSYM
