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
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>

#include <linux/pci.h>
//мое
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/netdevice.h>
#include <linux/device.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/string.h>

/*
** I am using the kernel 5.10.27-v7l. So I have set this as 510.
** If you are using the kernel 3.10, then set this as 310,
** and for kernel 5.1, set this as 501. Because the API proc_create()
** changed in kernel above v5.5.
**
*/
#define LINUX_KERNEL_VERSION  510
//#define WR_VALUE _IOW('a','a',int32_t*)
//#define RD_VALUE _IOR('a','b',int32_t*)

int32_t value = 0;
static int len = 1;


dev_t dev = 0;
static struct class *dev_class;
static struct proc_dir_entry *parent;
struct pci_dev *dev2;

/*
** Function Prototypes
*/
//static int      __init etx_driver_init(void);
//static void     __exit etx_driver_exit(void);

/***************** Procfs Functions *******************/
static ssize_t  show_dm_dirty_log_type(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t  show_device(struct file *filp, char __user *buffer, size_t length,loff_t * offset);

/*
** procfs operation sturcture
*/
static struct proc_ops ddlt_fops = {
        .proc_read = show_dm_dirty_log_type,
};

static struct proc_ops device_fops = {
        .proc_read = show_device
};

// Мои переменные
struct task_struct *g, *p;
/*
** This function will be called when we read the procfs file
*/
static ssize_t show_dm_dirty_log_type(struct file *filp, char __user *buffer, size_t length, loff_t * offset){
    if (len) {
        len=0;
    }
    else {
        len=1;
        return 0;
    }
    if(copy_to_user(buffer, "multiprocess\n", 13)){
        pr_err("Data Send : Err!\n");
    }
    return length;
}

static ssize_t show_device(struct file *filp, char __user *buffer, size_t length, loff_t * offset){
    pr_info("Start read_proc");
    if(len) {
        len=0;
    }
    else {
        len=1;
        return 0;
    }
    char arr[4000];
    char str[40];
    int i;
    int k = 0;
    ////pci_dev
    while ((dev2 = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, dev2))){
        sprintf(str, "pci found [%d]\tflags[%d]\n", (dev2->device), dev2->dev_flags);
        for (i = k; i < k + 40; i++){
            arr[i] = str[i - k];
        }
        k += 40;
        if (k >= 4000) break;
    }
    ////multiprocess
    //print
    if(copy_to_user(buffer, arr, k)){
        pr_err("Data Send : Err!\n");
    }
    return length;
}

/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
    /*Create proc directory. It will create a directory under "/proc" */
    parent = proc_mkdir("etx",NULL);
    if( parent == NULL )
    {
        pr_info("Error creating proc entry");
        class_destroy(dev_class);
        return -1;
    }

    /*Creating Proc entry under "/proc/etx/" */
    proc_create("dm_dirty_log_type", 0666, parent, &ddlt_fops);
    proc_create("device", 0666, parent, &device_fops);

    pr_info("Device Driver Insert...Done!!!\n");
    return 0;
}

/*
** Module exit function
*/
static void __exit etx_driver_exit(void)
{
    /* remove complete /proc/etx */
    proc_remove(parent);
    pr_info("Device Driver Remove...Done!!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("Simple Linux device driver (procfs)");
MODULE_VERSION("1.6");