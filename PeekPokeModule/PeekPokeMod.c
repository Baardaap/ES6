#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/device.h>

#include <mach/hardware.h>

#define sysfs_dir "peekpoke"
#define sysfs_file "data"
//#define RTC io_p2v(0x40024000)

#define sysfs_max_data_size 1024

static char sysfs_buffer[sysfs_max_data_size+1] = "Test Message\n";
static ssize_t used_buffer_size =0;

static ssize_t
sysfs_show(	struct device *dev,
			struct device_attribute *attr,
			char *buffer)
			{
				printk(KERN_INFO "sysfile_read(/sys/kernel/%s/%s) called\n", sysfs_dir, sysfs_file);
				return sprintf(buffer, "%s", sysfs_buffer);
			}
			
static ssize_t
sysfs_store(struct device *dev,
			struct device_attribute *attr,
			const char *buffer,
			size_t count)
			{
				used_buffer_size = count > sysfs_max_data_size ? sysfs_max_data_size :count;
				printk(KERN_INFO "sysfile_write (/sys/kernel/%s/%s) called\n",sysfs_dir,sysfs_file);
				memcpy(sysfs_buffer,buffer,used_buffer_size);
				sysfs_buffer[used_buffer_size] = '\0';
				return used_buffer_size;
			}

static DEVICE_ATTR(data, S_IWUGO | S_IRUGO, sysfs_show, sysfs_store);

static struct attribute *attrs[]={
	&dev_attr_data.attr,
	NULL
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *peekpoke_obj = NULL;

int __init sysfs_init(void)
{
	int result = 0;
	
	peekpoke_obj = kobject_create_and_add(sysfs_dir, kernel_kobj);
	if(peekpoke_obj == NULL)
	{
		printk (KERN_INFO "%s module failed to load: create/add failed \n", sysfs_file);
		return -ENOMEM;
	}
	
	result = sysfs_create_group(peekpoke_obj, &attr_group);
	if(result != 0)
	{
		printk (KERN_INFO "%s module failed to load: sysfs_create_group failed with result %d\n", sysfs_file, result);
		return -ENOMEM;
	}
	
	printk (KERN_INFO "/sys/kernel/%s/%s created\n", sysfs_dir, sysfs_file);
	return result;
}

void __exit sysfs_exit(void)
{
	kobject_put(peekpoke_obj);
	printk (KERN_INFO "/sys/kernel/%s/%s removed\n", sysfs_dir,sysfs_file);
}

module_init(sysfs_init);
module_exit(sysfs_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR(" test");
MODULE_DESCRIPTION("blah");
/*
int init_module(void)
{
	int val;
    uint32_t *a = (uint32_t*)  RTC;
    val = *a;
    printk("%d\n", val);
	return 0;
}

void cleanup_module(void)
{
	int val;
    uint32_t *a = (uint32_t*) RTC;
    val = *a;
    printk("%d\n", val);
}
*/
