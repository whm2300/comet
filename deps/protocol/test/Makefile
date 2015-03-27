a : main.o handle_account.o comet.pb.o
	g++ -g -o a main.o handle_account.o comet.pb.o ../libprotobuf.a

main.o : main.cc
	g++ -g -c main.cc

handle_account.o : handle_account.h handle_account.cc
	g++ -g -c handle_account.cc

comet.pb.o : comet.pb.h comet.pb.cc
	g++ -g -c comet.pb.cc

clean:
	rm -rf  main.o handle_account.o comet.pb.o a handle_account.h.gch
