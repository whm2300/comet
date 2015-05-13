include build.mk

all:
	mkdir -p $(OBJS_PATH)
	cd src/core && $(MAKE) 

.PHONY : clean
clean:
	rm -rf $(OBJS_PATH)
	cd src/core; make clean
