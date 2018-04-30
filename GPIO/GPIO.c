#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <mach/hardware.h>

#define P2_MUX_SET      0x40028028
#define P2_INP_STATE    0x4002801C
#define P2_DIR_SET      0x40028010

#define P2_EMC_D_ENABLE_MASK        0x00000008
#define P2_INPUTDIR_JOYSTICK_MASK   0x0000001F

/* static ssize_t sysfs_show(struct device *dev, struct device_attribute *attr, char *buffer)
{
    return 0;
}

static ssize_t sysfs_store(struct device *dev, struct device_attribute *attr, const char *buffer, size_t count)
{
    return 0;
}

static int device_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t device_read(struct file *file, char *buffer, size_t length, loff_t * offset)
{
    return 0;
}

static ssize_t device_write(struct file *file, const char *buff, size_t length, loff_t * off)
{
    return 0;
}

struct file_operations Fops =
{
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
};

static DEVICE_ATTR(data, S_IWUGO | S_IRUGO, sysfs_show, sysfs_store);
static struct attribute *attrs[] = { &dev_attr_data.attr, NULL};
static struct attribute_group attr_group = {.attrs = attrs,};
static struct kobject *hello_obj = NULL; */

int init_module (void)
{
    uint32_t *address = io_p2v(P2_MUX_SET);
    *address |= P2_EMC_D_ENABLE_MASK;         //Connect pins 19 - 31 from the EMC_D to the GPIO port 2
    
    address = io_p2v(P2_DIR_SET);
    *address |= P2_INPUTDIR_JOYSTICK_MASK;    //Set pins 2.0 - 2.4 on input mode
}

void cleanup_module(void)
{
    uint32_t joystickStates = io_p2v(P2_INP_STATE);
    joystickStates &= P2_INPUTDIR_JOYSTICK_MASK;
    printk(KERN_INFO "Joystick value: %x\n", joystickStates);
}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Konghon Choo & Ian Seelen");
MODULE_DESCRIPTION("A driver for the GPIO pins of the LPC3250 development board");