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


#include <linux/dm-dirty-log.h>
#include <linux/list.h>
#include <linux/module.h>


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



struct dm_dirty_log_type *dmDirtyLogType;
struct pci_dev *dev2;

/*
** Function Prototypes
*/
//static int      __init etx_driver_init(void);
//static void     __exit etx_driver_exit(void);

/***************** Procfs Functions *******************/
static ssize_t  show_dm_dirty_log_type(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t  show_device(struct file *filp, char __user *buffer, size_t length,loff_t * offset);

struct dm_dirty_log_type *myget_ret_log_type(const char *type_name);

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

    //get ddlt
    dmDirtyLogType = myget_ret_log_type("core");

    char *arr = kmalloc(4000, GFP_KERNEL);



//    arr = dmDirtyLogType->module->name;
//    arr = dmDirtyLogType->module->name;

    char * module_state_to_str(enum module_state moduleState)
    {
        switch(moduleState){
            case MODULE_STATE_LIVE: return "MODULE_STATE_LIVE";
            case MODULE_STATE_COMING: return "MODULE_STATE_COMING";
            case MODULE_STATE_GOING: return "MODULE_STATE_GOING";
        }
        return "null";
    }

    printk(KERN_INFO "dm_dirty_log_type name:%s\n", dmDirtyLogType->name);
    printk(KERN_INFO "module name:%s\n", dmDirtyLogType->module->name);
    enum module_state ms = dmDirtyLogType->module->state;
    char *state = module_state_to_str(ms);
    printk(KERN_INFO "module state:%s\n", state);
    printk(KERN_INFO "module version name:%s\n", dmDirtyLogType->module->version);
    printk(KERN_INFO "module srcversion name:%s\n", dmDirtyLogType->module->srcversion);
    printk(KERN_INFO "module num kernal param name:%d\n", dmDirtyLogType->module->num_kp);

//    struct dm_dirty_log *log;
//    list_head list = dmDirtyLogType->list;
//    list_for_each(log, list){
//        arr = log->context;
//    }


//    sprintf(arr,"%d",dmDirtyLogType->list);





    if(copy_to_user(buffer, arr, 13)){
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
    char *arr = kmalloc(4000, GFP_KERNEL);
    char *str = kmalloc(40, GFP_KERNEL);
    int i;
    int k = 0;
    ////pci_dev

    for_each_pci_dev(dev2){
//        struct fb_data_t foo_data;
//        strcpy(foo_data.name, "device name");

        char s[20]; // Для двузначного хватит и s[3] - не забываем о нулевом символе
        sprintf(s,"%d",dev2->device);

//        strcpy(foo_data.value, s);


        sprintf(str, "pci found [%d]\tinit_name[%s]\tinit_parent_name[%s]\n", (dev2->device), dev2->dev.init_name, dev2->dev.parent->init_name);

        printk(KERN_INFO "pci found device:%d\n", dev2->device);
        printk(KERN_INFO "init name:%s\n", dev2->dev.init_name);
        printk(KERN_INFO "(parent) init name:%s\n", dev2->dev.parent->init_name);
        printk(KERN_INFO "(kobject) name:%s\n", dev2->dev.kobj.name);
        printk(KERN_INFO "      parent name:%s\n", dev2->dev.kobj.parent->name);
        printk(KERN_INFO "(type) name:%s\n", dev2->dev.type->name);
    }



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