CC=gcc
CXX=g++

CFLAGS :=
CFLAGS += -g -Wall
CFLAGS += -lpthread
CFLAGS += -I "/root/Documents/code/eglcomet_thread/deps/libevent-2.0.21-stable/include"

CLIBS :=
CLIBS += "/root/Documents/code/eglcomet_thread/deps/libevent-2.0.21-stable/.libs/libevent.a"
CLIBS += -lrt -lpthread

OBJS_PATH=objs
EXE_NAME=eglcomet
