obj-m += ext.o

EXTRA_CFLAGS += -I${PWD}/include


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test:
	sudo rmmod ext.ko
	sudo insmod ext.ko
ins:
	sudo insmod ext.ko