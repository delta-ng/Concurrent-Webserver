# An admittedly primitive Makefile
# To compile, type "make" or make "all"
# To remove files, type "make clean"

CC = gcc
CFLAGS = -Wall
OBJS = thread_pool.o wserver.o wclient.o request.o io_helper.o

.SUFFIXES: .c .o 

# all: thread wserver wclient spin.cgi
all: thread wserver wclient spin.cgi

thread: thread_pool.o 
	$(CC) $(CFLAGS) -pthread -o thread_pool thread_pool.c request.c io_helper.c

# thread_pool: thread_pool.o
# 	$(CC) $(CFLAGS) -pthread -o thread_pool thread_pool.o
# thread: thread_pool.o request.o io_helper.o
#	$(CC) $(CFLAGS) -pthread -o thread_pool thread_pool.o request.o io_helper.o

wserver: wserver.o request.o io_helper.o thread_pool.o
	$(CC) $(CFLAGS) -pthread -o wserver wserver.c request.c io_helper.c thread_pool.c
#wclient: wclient.o io_helper.o
#	$(CC) $(CFLAGS) -pthread -o wclient wclient.c io_helper.c

spin.cgi: spin.c
	$(CC) $(CFLAGS) -o spin.cgi spin.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f $(OBJS) wserver wclient spin.cgi
