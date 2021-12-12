/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple Linux device driver (procfs)
*
*  \author     EmbeTronicX
* 
*  \Tested with Linux raspberrypi 5.10.27-v7l-embetronicx-custom+
*
* *******************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>
#include <linux/proc_fs.h>
#include <linux/pci.h>
#include <linux/device.h>
#include <linux/dm-dirty-log.h>
#include <linux/kallsyms.h>
//#include <linux/dm-log-userspace.h>


/* 
** I am using the kernel 5.10.27-v7l. So I have set this as 510.
** If you are using the kernel 3.10, then set this as 310,
** and for kernel 5.1, set this as 501. Because the API proc_create()
** changed in kernel above v5.5.
**
*/
#define LINUX_KERNEL_VERSION  510

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

int32_t value = 0;

static int len = 1;


dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
static struct proc_dir_entry *parent;
static struct proc_dir_entry *parent_device;
static struct proc_dir_entry *parent_dm_dirty_log_type;

char *etx_array;


struct dm_dirty_log_type *get_dm_dirty_log_type(const char *type_name);

static dm_dirty_log_type *ddlt;

static void fill_ddlt(const char *type_name){
    ddlt = get_dm_dirty_log_type(type_name)
}

static struct device *mydec;

//static char *ccccccc = "null";

//static struct dm_dirty_log *dev1;

struct pci_dev *dev2;

//extern struct dm_dirty_log_type *myget_log_type(const char *type_name);


/*
** Function Prototypes
*/
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);

/*************** Driver Functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t * off);
static ssize_t etx_write(struct file *filp, const char *buf, size_t len, loff_t *off);
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/***************** Procfs Functions *******************/
static int open_proc(struct inode *inode, struct file *file);
static int release_proc(struct inode *inode, struct file *file);
static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length, loff_t * offset);
static ssize_t read2_proc(struct file *filp, char __user *buffer, size_t length, loff_t * offset);
static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t *off);


struct proc_dir_entry *proc_file_entry;


/*
** File operation sturcture
*/
static struct file_operations fops =
        {
                .owner          = THIS_MODULE,
                .read           = etx_read,
                .write          = etx_write,
                .open           = etx_open,
                .unlocked_ioctl = etx_ioctl,
                .release        = etx_release,
        };


#if (LINUX_KERNEL_VERSION > 505)

/*
** procfs operation sturcture
*/
static struct proc_ops proc_fops = {
        .proc_open = open_proc,
        .proc_read = read_proc,
        .proc_write = write_proc,
        .proc_release = release_proc
};
static struct proc_ops proc_fops2 = {
        .proc_open = open_proc,
        .proc_read = read2_proc,
        .proc_write = write_proc,
        .proc_release = release_proc
};

#else //LINUX_KERNEL_VERSION > 505

/*
** procfs operation sturcture
*/
static struct file_operations proc_fops = {
        .open = open_proc,
        .read = read_proc,
        .write = write_proc,
        .release = release_proc
};

#endif //LINUX_KERNEL_VERSION > 505

/*
** This function will be called when we open the procfs file
*/
static int open_proc(struct inode *inode, struct file *file) {
    pr_info("proc file opend.....\t");
    return 0;
}

/*
** This function will be called when we close the procfs file
*/
static int release_proc(struct inode *inode, struct file *file) {
    pr_info("proc file released.....\n");
    return 0;
}

/*
** This function will be called when we read the procfs file
*/
static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length, loff_t * offset){
    pr_info("proc file read.....\n");
    if(len){
        len = 0;
    }
    else{
        len = 1;
        return 0;
    }
    if(copy_to_user(buffer, etx_array, sizeof(etx_array))){
        pr_err("Data Send : Err!\n");
    }
    return length;
}

/*
** This function will be called when we write the procfs file
*/
static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t *off) {
    pr_info("proc file wrote.....\n");

//    if (copy_from_user(etx_array, buff, len)) {
//        pr_err("Data Write : Err!\n");
//    }

    return len;
}

/*
** This function will be called when we open the Device file
*/
static int etx_open(struct inode *inode, struct file *file) {
    pr_info("Device File Opened...!!!\n");
    return 0;
}

/*
** This function will be called when we close the Device file
*/
static int etx_release(struct inode *inode, struct file *file) {
    pr_info("Device File Closed...!!!\n");
    return 0;
}

/*
** This function will be called when we read the Device file
*/
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
    pr_info("Read function\n");
    return 0;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
    pr_info("Write Function\n");
    return len;
}

/*
** This function will be called when we write IOCTL on the Device file
*/
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case WR_VALUE:
            if (copy_from_user(&value, (int32_t *) arg, sizeof(value))) {
                pr_err("Data Write : Err!\n");
            }
            pr_info("Value = %d\n", value);
            break;
        case RD_VALUE:
            if (copy_to_user((int32_t *) arg, &value, sizeof(value))) {
                pr_err("Data Read : Err!\n");
            }
            break;
        default:
            pr_info("Default\n");
            break;
    }
    return 0;
}

/*
** Module Init function
*/
static int __init etx_driver_init(void) {

    /*Allocating Major number*/
    if ((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) < 0) {
        pr_info("Cannot allocate major number\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

    /*Creating cdev structure*/
    cdev_init(&etx_cdev, &fops);

    /*Adding character device to the system*/
    if ((cdev_add(&etx_cdev, dev, 1)) < 0) {
        pr_info("Cannot add the device to the system\n");
        goto r_class;
    }

    /*Creating struct class*/
    if ((dev_class = class_create(THIS_MODULE, "etx_class")) == NULL) {
        pr_info("Cannot create the struct class\n");
        goto r_class;
    }

    /*Creating device*/
    if ((mydec = device_create(dev_class, NULL, dev, NULL, "etx_device")) == NULL) {
        pr_info("Cannot create the Device 1\n");
        goto r_device;
    }

    /*Create proc directory. It will create a directory under "/proc" */
    parent = proc_mkdir("lab2", NULL);
    parent_device = proc_mkdir("devices", parent);
    parent_dm_dirty_log_type = proc_mkdir("dm_dirty_log_types", parent);

    if (parent == NULL) {
        pr_info("Error creating proc entry");
        goto r_device;
    }
    if (parent_device == NULL) {
        pr_info("Error creating proc entry");
        goto r_device;
    }
    if (parent_dm_dirty_log_type == NULL) {
        pr_info("Error creating proc entry");
        goto r_device;
    }


    fill_ddlt("core");



    /*Creating Proc entry under "/proc/etx/" */
//    proc_create("etxmyproccess", 0666, parent, &proc_fops);


//    printk(KERN_INFO "dm_dirty_log_type:%s\n", my_get_userspace_type().name);

    proc_file_entry2 = proc_create(s, 0666, parent_device, &proc_fops2);

    for_each_pci_dev(dev2){
//        struct fb_data_t foo_data;
//        strcpy(foo_data.name, "device name");

        char s[20]; // Для двузначного хватит и s[3] - не забываем о нулевом символе
        sprintf(s,"%d",dev2->device);

//        strcpy(foo_data.value, s);

        proc_file_entry = proc_create(s, 0666, parent_device, &proc_fops);



        printk(KERN_INFO "pci found device:%d\n", dev2->device);
        printk(KERN_INFO "init name:%s\n", dev2->dev.init_name);
        printk(KERN_INFO "(parent) init name:%s\n", dev2->dev.parent->init_name);
        printk(KERN_INFO "(kobject) name:%s\n", dev2->dev.kobj.name);
        printk(KERN_INFO "      parent name:%s\n", dev2->dev.kobj.parent->name);
        printk(KERN_INFO "(type) name:%s\n", dev2->dev.type->name);
    }



    pr_info("Device Driver Insert...Done!!!\n");
    return 0;

    r_device:
    class_destroy(dev_class);
    r_class:
    unregister_chrdev_region(dev, 1);
    return -1;
}

/*
** Module exit function
*/
static void __exit

etx_driver_exit(void) {
    /* Removes single proc entry */
    //remove_proc_entry("etx/etx_proc", parent);

    /* remove complete /proc/etx */
    proc_remove(parent);

    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device Driver Remove...Done!!!\n");
}


module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("KirillShakhov <kirill.shakhov14@gmail.com>");
MODULE_DESCRIPTION("Simple Linux device driver (procfs) dirty region log");
MODULE_VERSION("1.6");
