# An admittedly primitive Makefile
# To compile, type "make" or make "all"
# To remove files, type "make clean"

CC = gcc
CFLAGS = -w
OBJS = thread_pool.o wserver.o wclient.o request.o io_helper.o

.SUFFIXES: .c .o 

all: wserver wclient spin.cgi

# thread: thread.o 
# 	$(CC) $(CFLAGS) -pthread -o thread_pool thread.o

# thread_pool: thread_pool.o
# 	$(CC) $(CFLAGS) -pthread -o thread_pool thread_pool.o
# thread: thread_pool.o request.o io_helper.o
# 	$(CC) $(CFLAGS) -pthread -o thread_pool thread_pool.o request.o io_helper.o

wserver: wserver.o request.o io_helper.o thread_pool.o
	$(CC) $(CFLAGS) -pthread -o wserver wserver.o request.o io_helper.o thread_pool.o

wclient: wclient.o io_helper.o
	$(CC) $(CFLAGS) -pthread -o wclient wclient.o io_helper.o thread_pool.o

spin.cgi: spin.c
	$(CC) $(CFLAGS) -o spin.cgi spin.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f $(OBJS) wserver wclient spin.cgi
