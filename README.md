# Overview

In this assignment, you will be developing a concurrent web server. To
simplify this project, we are providing you with the code for a non-concurrent
(but working) web server. This basic web server operates with only a single
thread; it will be your job to make the web server multi-threaded so that it
can handle multiple requests at the same time.

The goals of this project are:
- To learn the basic architecture of a simple web server
- To learn how to add concurrency to a non-concurrent system
- To learn how to read and modify an existing code base effectively

Useful reading from [OSTEP](http://ostep.org) includes:
- [Intro to threads](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf)
- [Using locks](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf)
- [Producer-consumer relationships](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf)
- [Server concurrency architecture](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-events.pdf)

## Command-line Parameters

Your C program must be invoked exactly as follows:

```sh
prompt> ./wserver [-d basedir] [-p port] [-t threads]
```

The command line arguments to your web server are to be interpreted as
follows.

- **basedir**: this is the root directory from which the web server should
  operate. The server should try to ensure that file accesses do not access
  files above this directory in the file-system hierarchy. Default: current
  working directory (e.g., `.`).
- **port**: the port number that the web server should listen on; the basic web
  server already handles this argument. Default: 10000.
- **threads**: the number of worker threads that should be created within the web
  server. Must be a positive integer. Default: 1.

For example, you could run your program as:
```
prompt> server -d . -p 8003 -t 8
```

In this case, your web server will listen to port 8003, create 8 worker threads for
handling HTTP requests.

# Source Code Overview

We recommend understanding how the code that we gave you works.  We provide
the following files:

- [`wserver.c`](https://github.com/remzi-arpacidusseau/ostep-projects/blob/master/concurrency-webserver/src/wserver.c): Contains `main()` for the web server and the basic serving loop.
- [`request.c`](https://github.com/remzi-arpacidusseau/ostep-projects/blob/master/concurrency-webserver/src/request.c): Performs most of the work for handling requests in the basic
  web server. Start at `request_handle()` and work through the logic from
  there. 
- [`io_helper.h`](https://github.com/remzi-arpacidusseau/ostep-projects/blob/master/concurrency-webserver/src/io_helper.h) and [`io_helper.c`](https://github.com/remzi-arpacidusseau/ostep-projects/blob/master/concurrency-webserver/src/io_helper.c): Contains wrapper functions for the system calls invoked by
  the basic web server and client. The convention is to add `_or_die` to an
  existing call to provide a version that either succeeds or exits. For
  example, the `open()` system call is used to open a file, but can fail for a
  number of reasons. The wrapper, `open_or_die()`, either successfully opens a
  file or exists upon failure. 
- [`wclient.c`](https://github.com/remzi-arpacidusseau/ostep-projects/blob/master/concurrency-webserver/src/wclient.c): Contains main() and the support routines for the very simple
  web client. To test your server, you may want to change this code so that it
  can send simultaneous requests to your server. By launching `wclient`
  multiple times, you can test how your server handles concurrent requests.
- [`spin.c`](https://github.com/remzi-arpacidusseau/ostep-projects/blob/master/concurrency-webserver/src/spin.c): A simple CGI program. Basically, it spins for a fixed amount
  of time, which you may useful in testing various aspects of your server.  
- [`Makefile`](https://github.com/remzi-arpacidusseau/ostep-projects/blob/master/concurrency-webserver/src/Makefile): We also provide you with a sample Makefile that creates
  `wserver`, `wclient`, and `spin.cgi`. You can type make to create all of 
  these programs. You can type make clean to remove the object files and the
  executables. You can type make server to create just the server program,
  etc. As you create new files, you will need to add them to the Makefile.

The best way to learn about the code is to compile it and run it. Run the
server we gave you with your preferred web browser. Run this server with the
client code we gave you. You can even have the client code we gave you contact
any other server that speaks HTTP. Make small changes to the server code
(e.g., have it print out more debugging information) to see if you understand
how it works.


