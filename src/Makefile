include ../build.mk

OBJS = ../$(OBJS_PATH)/eglcomet.o ../$(OBJS_PATH)/server.o ../$(OBJS_PATH)/log.o ../$(OBJS_PATH)/shared_data.o ../$(OBJS_PATH)/work_thread.o ../$(OBJS_PATH)/analyse_req.o ../$(OBJS_PATH)/comet.pb.o ../$(OBJS_PATH)/subscriber.o
CFLAGS += -lboost_system -lboost_filesystem

all:$(OBJS)
	${CXX} -o ../$(EXE_NAME) ${CFLAGS} ${OBJS} ${CLIBS}

../$(OBJS_PATH)/eglcomet.o:eglcomet.cc
	${CXX} -c ${CFLAGS} eglcomet.cc -o ../$(OBJS_PATH)/eglcomet.o

../$(OBJS_PATH)/server.o:server.h server.cc
	${CXX} -c ${CFLAGS} server.cc -o ../$(OBJS_PATH)/server.o

../$(OBJS_PATH)/log.o:log.h log.cc
	${CXX} -c ${CFLAGS} log.cc -o ../$(OBJS_PATH)/log.o

../$(OBJS_PATH)/shared_data.o:shared_data.h shared_data.cc
	${CXX} -c ${CFLAGS} shared_data.cc -o ../$(OBJS_PATH)/shared_data.o

../$(OBJS_PATH)/work_thread.o:work_thread.h work_thread.cc
	${CXX} -c ${CFLAGS} work_thread.cc -o ../$(OBJS_PATH)/work_thread.o


../$(OBJS_PATH)/analyse_req.o:analyse_req.h analyse_req.cc
	${CXX} -c ${CFLAGS} analyse_req.cc -o ../$(OBJS_PATH)/analyse_req.o

../$(OBJS_PATH)/comet.pb.o:comet.pb.h comet.pb.cc
	${CXX} -c ${CFLAGS} comet.pb.cc -o ../$(OBJS_PATH)/comet.pb.o

../$(OBJS_PATH)/subscriber.o:subscriber.h subscriber.cc
	${CXX} -c ${CFLAGS} subscriber.cc -o ../$(OBJS_PATH)/subscriber.o

clean:
	rm -f ../$(EXE_NAME)
