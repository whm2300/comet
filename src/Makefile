include ../build.mk

OBJS = ../$(OBJS_PATH)/eglcomet.o ../$(OBJS_PATH)/server.o ../$(OBJS_PATH)/log.o
CFLAGS += -lboost_system -lboost_filesystem

all:$(OBJS)
	${CXX} -o ../$(EXE_NAME) ${CFLAGS} ${OBJS} ${CLIBS}

../$(OBJS_PATH)/eglcomet.o:eglcomet.cc
	${CXX} -c ${CFLAGS} eglcomet.cc -o ../$(OBJS_PATH)/eglcomet.o
../$(OBJS_PATH)/server.o:server.h server.cc
	${CXX} -c ${CFLAGS} server.cc -o ../$(OBJS_PATH)/server.o
../$(OBJS_PATH)/log.o:log.h log.cc
	${CXX} -c ${CFLAGS} log.cc -o ../$(OBJS_PATH)/log.o

clean:
	rm -f ../$(EXE_NAME)
