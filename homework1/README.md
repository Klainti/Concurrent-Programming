# Assigment 

1.1 FIFO PIPE

Implemention single direction fifo pipe for communication between 2 threads as a separate piece of software with the functions:
- void pipe_init(int size)
- void pipe_write(char c)
- void pipe_close()
- int pipe_read(char *c)

Think about race conditions that might occur (simultaneous pipe write/read) and compare your perfomance with system pipe.
Which have better perfomance (system pipe or concurrent implement of FIFO pipe)
