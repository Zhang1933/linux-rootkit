objs := server.o
deps := $(objs:.o=.d)

# 自动生成依赖：https://stackoverflow.com/questions/39002087/about-the-gnu-make-dependency-files-d
CPPFLAGS += -MMD -MP 

#LDLIBS += -lncurses

server: $(objs)


.PHONY: clean
clean:
	-rm *.o *.d server


-include $(deps)
