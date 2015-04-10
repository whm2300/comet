CFLAGS :=
CFLAGS += -I /root/Documents/code/eglcomet_thread/test_script/mongo-2.4 -Wall
CFLAGS += -lboost_filesystem -lboost_program_options -lboost_thread-mt
all: log.o comet.pb.o
	g++ $(CFLAGS) -o receive comet.pb.o receive.cc log.o  /root/Documents/code/eglcomet_thread/test_script/mongo-2.4/libmongoclient.a libprotobuf.a /root/Documents/code/eglcomet_thread/deps/libevent-2.0.21-stable/.libs/libevent.a
	g++ $(CFLAGS) -o send comet.pb.o send.cc log.o  /root/Documents/code/eglcomet_thread/test_script/mongo-2.4/libmongoclient.a libprotobuf.a /root/Documents/code/eglcomet_thread/deps/libevent-2.0.21-stable/.libs/libevent.a

log.o: log.h log.cc
	g++ -c log.cc

comet.pb.o : comet.pb.h comet.pb.cc
	g++ -c comet.pb.cc

clean:
	rm -rf receive
