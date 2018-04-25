#include <mach/hardware.h>

#define P2_MUX_SET  0x40028028

static ssize_t sysfs_show(struct device *dev, struct device_attribute *attr, char *buffer)
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
static struct kobject *hello_obj = NULL;

int init_module (void)
{
    io_p2v(P2_MUX_SET) &= 0x04;
}

void cleanup_module(void)
{

}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Konghon Choo & Ian Seelen");
MODULE_DESCRIPTION("A driver for the GPIO pins of the LPC3250 development board");
