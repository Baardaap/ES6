#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/device.h>

#define sysfs_dir  "buffer"
#define sysfs_file "data"

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
        if(IO == 'I')
            result = findAndSetPin(jumper,indexpin,SET_PORT,SET_I);
        else if (IO == 'O')
            result = findAndSetPin(jumper,indexpin,SET_PORT,SET_O);
        else 
            printk(KERN_INFO "Input error! -> enter I or O");

    }

    memcpy(sysfs_buffer, buffer, used_buffer_size);
    sysfs_buffer[used_buffer_size] = '\0'; 

    return used_buffer_size;
}