#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xe99f1184, "module_layout" },
	{ 0xe2e752ec, "cdev_del" },
	{ 0x62253ab0, "device_destroy" },
	{ 0x6d29b61c, "proc_remove" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x77bc0832, "class_destroy" },
	{ 0x2cedb95, "proc_create" },
	{ 0x39f8bd0a, "proc_mkdir" },
	{ 0x2717b894, "device_create" },
	{ 0xa17bf155, "__class_create" },
	{ 0x2186ba12, "cdev_add" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0xb44ad4b3, "_copy_to_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x22ebfd60, "pci_get_device" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "BDD79A9003206B0E28D8818");
