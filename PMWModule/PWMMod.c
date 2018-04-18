#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <mach/hardware.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Konghon Choo & Ian Seelen");
MODULE_DESCRIPTION("PWMMod");

#define NAME                "PWMdev"

#define PWMCLOCK_REG		0x400040B8
#define PWMCLOCK_VAL		0x115
#define LCDCONFIG_REG		0x40004054
#define LCDCONFIG_VAL		0

int major;

static ssize_t device_read()
{
    return 0;
}

static ssize_t device_write()
{
    return 0;
}

static int device_open()
{
    return 0;
}

static int device_release()
{
    return 0;
}

static struct file_operations fops =
{
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

int init_module(void)
{
    major = register_chrdev(0, NAME, &fops);
    if(major < 0)
    {
        printk("Registering device failed\n");
        return major;
    }
    printk("Registering device %s on major: %d succeeded\n", NAME, major);
    printk("Please use mknod to create the appropriate files\n"); 

    *(uint32_t*)(io_p2v(PWMCLOCK_REG)) = PWMCLOCK_VAL;
    *(uint32_t*)(io_p2v(LCDCONFIG_REG)) = LCDCONFIG_VAL;

    return 0;
}

void cleanup_module(void)
{
    unregister_chrdev(major, NAME);
}