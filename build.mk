CC=gcc
CXX=g++

CFLAGS :=
CFLAGS += -g -Wall
CFLAGS += -lpthread
CFLAGS += -I "/root/Documents/code/eglcomet_thread/deps/libevent-2.0.21-stable/include"
CFLAGS += -I "/root/Documents/code/eglcomet_thread/deps/redis"
CFLAGS += -std=c++0x

CLIBS :=
CLIBS += "/root/Documents/code/eglcomet_thread/deps/libevent-2.0.21-stable/.libs/libevent.a"
CLIBS += "/root/Documents/code/eglcomet_thread/deps/protocol/libprotobuf.a"
CLIBS += "/root/Documents/code/eglcomet_thread/deps/redis/libhiredis.so"
CLIBS += -lrt -lpthread

OBJS_PATH=objs
EXE_NAME=eglcomet
