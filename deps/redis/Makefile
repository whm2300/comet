all : example-libevent.o
	gcc -o a example-libevent.o -lrt  -I /root/Documents/code/eglComet/deps/libevent-2.0.21-stable -I /root/Documents/code/eglComet/deps/redis /root/Documents/code/eglComet/deps/libevent-2.0.21-stable/.libs/libevent.a ./libhiredis.so

example-libevent.o:
	gcc -c -g example-libevent.c -I /root/Documents/code/eglComet/deps/libevent-2.0.21-stable -I /root/Documents/code/eglComet/deps/redis

clean:
	rm -f example-libevent.o
