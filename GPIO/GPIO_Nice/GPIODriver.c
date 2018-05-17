#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/device.h>
#include  "GPIOLPC.h"
#include  "LPC3250pins.h"

#define DEVICE_NAME "GPIOnode"
#define MAJORNUMBER 99 

#define sysfs_dir  "buffer"
#define sysfs_file "data"

int jumperToRead = -1;
int pinToRead = -1;

struct file_operations Fops =
{
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
};

static ssize_t sysfs_show(struct device *dev, struct device_attribute *attr, char *buffer)
{
    printk(KERN_INFO "sysfile_read (/sys/kernel/%s/%s) called\n", sysfs_dir, sysfs_file);
    
    return sprintf(buffer, "%s", sysfs_buffer);
}

static ssize_t sysfs_store(struct device *dev, struct device_attribute *attr, const char *buffer, size_t count)
{
    int jumper = 0;
    int indexpin = 0;
    char IO = '\0';
    int result = 0;
    used_buffer_size = count > sysfs_max_data_size ? sysfs_max_data_size : count; 
    printk(KERN_INFO "sysfile_write (/sys/kernel/%s/%s) called, buffer: %s, count: %zu \n", sysfs_dir, sysfs_file, buffer, count);
    
    result = sscanf(buffer,"J%i.%i %c", &jumper, &indexpin, &IO);
    if(result != 3)
    {
        printk(KERN_INFO "Input error!"); 
    }
    else
    {
        switch(jumper) 
        {
            case 1:
                if(IO == 'I'|| IO == 'O')
                    setPinDir(indexpin, IO, J1list);
                else 
                    printk(KERN_INFO "Input error! -> enter I or O");

            break;

            case 2:
                if(IO == 'I'|| IO == 'O')
                    setPinDir(indexpin, IO, J2list);
                else 
                    printk(KERN_INFO "Input error! -> enter I or O");

            break;

            case 3:
                if(IO == 'I'|| IO == 'O')
                    setPinDir(indexpin, IO, J3list);
                else 
                    printk(KERN_INFO "Input error! -> enter I or O");
            break;

            default:
                printk(KERN_INFO "Input error! -> enter 1-3 for the jumper")
            break;
        }

    }

    memcpy(sysfs_buffer, buffer, used_buffer_size);
    sysfs_buffer[used_buffer_size] = '\0'; 

    return used_buffer_size;
}

static int device_open(struct inode *inode, struct file *file)
{
    int tempminor = iminor(inode);
    file->private_data = (void*)tempminor;
    if (Device_Open)
    {
        return -EBUSY;
    }
    Device_Open++;
    try_module_get(THIS_MODULE);
    
    return 0;
}

static int device_release(struct inode *inode, struct file *file) 
{
    file->private_data = NULL;
    Device_Open--;   
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_read(struct file *file,   
                           char *buffer,        
                           size_t length,      
                           loff_t * offset)
{
    int result = -1; 
    int i;
    if(jumperToRead == -1 || pinToRead ==  -1)
    {
        printk(KERN_INFO "Read Commando not executed!");
        return 0;
    }
    else
    {
        switch(jumperToRead)
        {
        case 1:
            pinToRead(J1list);
        break;

        case 2:
            pinToRead(J1list);
        break;

        case 3:
            pinToRead(J1list);
        break;
        }
    }
    return 0;
}

static ssize_t device_write(struct file *file, const char *buff, size_t length, loff_t * off)
{
    int minornumber = (int)file->private_data;
    int result = 0; 
    int jumper = 0;
    int indexpin = 0;
    char value = '\0';

    result = sscanf(buff,"J%i.%i %c", &jumper, &indexpin, &value); 
    if (result != 3)
    {
         printk(KERN_INFO "Input error!"); 
    } else
    {   
        ports_t port;
        if(jumper ==  1)
            port = J1list;
        else if (jumper == 2)
            port = J2list;
        else if (jumper  == 3)
             port = J3list;

        if(minornumber == 0) 
        {   
            if(value == 'H')
                setPinDir(indexpin, 'O',port);
            else if (value == 'L')
                setPinDir(indexpin, 'O',port);
            else if (value == 'R')
            {
                jumperToRead = jumper;
                pinToRead = indexpin;
                setPinDir(indexpin, 'I',port);
            }
            else 
                printk(KERN_INFO "Input error! -> enter H (high) L (low) R (read)");
        }
        
    }
    return length;
}

static DEVICE_ATTR(data, S_IWUGO | S_IRUGO, sysfs_show, sysfs_store);
static struct attribute *attrs[] = { &dev_attr_data.attr, NULL};
static struct attribute_group attr_group = {.attrs = attrs,};
static struct kobject *gpio_obj = NULL;

int init_module (void)
{
    int result = 0;
    int majorNr = register_chrdev (MAJORNUMBER, DEVICE_NAME, &Fops);
    gpio_obj = kobject_create_and_add(sysfs_dir, kernel_kobj);
    if (gpio_obj == NULL)
    {
        printk (KERN_INFO "%s module failed to load: kobject_create_and_add failed\n", sysfs_file);
        return -ENOMEM;
    }

    result = sysfs_create_group(gpio_obj, &attr_group);
    if (result != 0)
    {
        printk (KERN_INFO "%s module failed to load: sysfs_create_group failed with result %d\n", sysfs_file, result);
        kobject_put(gpio_obj);
        return -ENOMEM;
    }

    printk(KERN_INFO "/sys/kernel/%s/%s created\n", sysfs_dir, sysfs_file);
    if (majorNr < 0)
    {
        printk(KERN_ALERT "Registering char device failed with %d\n", MAJORNUMBER);
        return majorNr;
    }
    printk(KERN_INFO "I was assigned major number %d. To talk to\n", MAJORNUMBER);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, MAJORNUMBER);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    initGPIO();

    return result;
}

void cleanup_module(void)
{
    unregister_chrdev(MAJORNUMBER, DEVICE_NAME);
    kobject_put(hello_obj);
    printk (KERN_INFO "/sys/kernel/%s/%s removed\n", sysfs_dir, sysfs_file);
}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Ian en Kong");
MODULE_DESCRIPTION("GPIO");
