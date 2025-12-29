#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

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
	{ 0x46b0be46, "single_open" },
	{ 0xc00e2b80, "seq_printf" },
	{ 0x8d522714, "__rcu_read_lock" },
	{ 0xa54c0f5b, "init_task" },
	{ 0x2469810f, "__rcu_read_unlock" },
	{ 0x6423a399, "init_user_ns" },
	{ 0x2c015e41, "from_kuid" },
	{ 0x63cd0125, "remove_proc_entry" },
	{ 0xc1d9b323, "seq_read" },
	{ 0x7369f212, "seq_lseek" },
	{ 0x8e66928c, "single_release" },
	{ 0x479b90bb, "param_ops_int" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x2dbde678, "proc_create" },
	{ 0x3f66a26e, "register_kprobe" },
	{ 0x122c3a7e, "_printk" },
	{ 0xbb10e61d, "unregister_kprobe" },
	{ 0xe2fd41e5, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "759636BBC49AECBAA4681A1");
