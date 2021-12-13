obj-m += lab.o

EXTRA_CFLAGS += -I${PWD}/include


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test:
	sudo rmmod lab.ko
	sudo insmod lab.ko
ins:
	sudo insmod lab.ko