export CC=gcc
export CXX=g++

export PROJECT_DIR := $(shell /bin/pwd)

export CXXFLAGS :=
CXXFLAGS += -g -Wall
CXXFLAGS += -std=c++0x
CXXFLAGS += -I $(PROJECT_DIR)/src/include
#CXXFLAGS += -I "/usr/local/include/boost"

export LDFLAGS :=
LDFLAGS += $(PROJECT_DIR)/src/lib/libevent.so
LDFLAGS += $(PROJECT_DIR)/src/lib/libhiredis.so
LDFLAGS += -lrt -lpthread -lprotobuf
LDFLAGS += -lboost_system -lboost_filesystem

export OBJS_PATH = $(PROJECT_DIR)/objs
export EXE_NAME=eglcomet
