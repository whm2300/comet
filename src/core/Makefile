OBJS = $(OBJS_PATH)/eglcomet.o $(OBJS_PATH)/server.o \
	   $(OBJS_PATH)/log.o $(OBJS_PATH)/shared_data.o \
	   $(OBJS_PATH)/work_thread.o $(OBJS_PATH)/analyse_req.o \
	   $(OBJS_PATH)/comet.pb.o $(OBJS_PATH)/subscriber.o

$(EXE_NAME) : $(OBJS)
	${CXX} -o $(PROJECT_DIR)/$(EXE_NAME) $(LDFLAGS) $(OBJS)

$(OBJS_PATH)/%.o : %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY : clean
clean :
	rm -f $(PROJECT_DIR)/$(EXE_NAME)
