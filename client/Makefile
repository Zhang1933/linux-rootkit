objs := client.o
deps := $(objs:.o=.d)

# auto generate dependency  https://stackoverflow.com/questions/39002087/about-the-gnu-make-dependency-files-d
CPPFLAGS += -MMD -MP 

client: $(objs)


.PHONY: clean
clean:
	-rm *.o *.d server


-include $(deps)
