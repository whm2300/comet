include build.mk

all:
	mkdir -p $(OBJS_PATH)
	cd src; make

clean:
	rm -rf $(OBJS_PATH)
	cd src; make clean
