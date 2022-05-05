obj-m += server.o
server-objs := server_main.o netfilterhook.o show_hide.o hook_syscall.o  

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
