//
// Created by Кирилл Шахов on 12.12.2021.
//

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/dm-dirty-log.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KirillShakhov");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");


struct dm_dirty_log_type *get_dm_dirty_log_type(const char *type_name);

static struct dm_dirty_log_type *ddlt;

static void fill_ddlt(const char *type_name){
    ddlt = get_dm_dirty_log_type(type_name);
}

static int __init lkm_example_init(void) {
    printk(KERN_INFO "Hello, World!\n");
    fill_ddlt("core");
    return 0;
}

static void __exit lkm_example_exit(void) {
    printk(KERN_INFO "Goodbye, World!\n");
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);
