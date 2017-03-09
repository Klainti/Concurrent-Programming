# Assigment 

1.1 FIFO PIPE

Implemention single direction fifo pipe for communication between 2 threads as a separate piece of software with the functions:
- void pipe_init(int size)
- void pipe_write(char c)
- void pipe_close()
- int pipe_read(char *c)

Think about race conditions that might occur (simultaneous pipe write/read) and compare your perfomance with system pipe.
Which have better perfomance (system pipe or concurrent implement of FIFO pipe)

1.2 Parallel fractal calculation

Take the code of MandelCalc and implement in parallel.

![alt tag](http://i.imgur.com/bHYZEFO.png)

1.3 Quicksort
Implement a parallel and recursive edition of quicksort with threads!

Programming in C, using pthreads library
