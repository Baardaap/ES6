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
				printk(KERNINFO "sysfile_read(/sys/kernel/%s/%s) called", sysfs_dir, sysfs_file);
				return sprintf(buffer, "/s", sysfs_buffer);
			}
			
static DEVICE_ATTR(data, S_IWUGO | S_IRUGO, sysfs_show, sysfs_store);

static struct attribute *attrs[]={
	.&dev_attr_data.attr,
	NULL
};
static struct kobject *peekpoke_obj = NULL;

int __init sysfs_init(void)
{
	peekpoke_obj = kboject_create_and_add(sysfs_dir, kernel_kobj);
	if(peekpoke_obj == NULL)
	{
		printk (KERN_INFO "%s module failed to load: create/add failed \n", sysfs_file);
		return -ENOMEM;
	}
	
	result = sysfs_create_group(peekpoke_obj, &attr_group);
	if(result != 0)
	{
		printk (KERN_INFO "%s module failed to load: sysfs_create_group failed with result %d\n", sysfs_file, result):
		return _ENOMEM;
	}
	
	printk (KERNINFO "/sys/kernel/%s/%s created\n", sysfs-dir, sysfs_file);
	return result;
}

module_init(sysfs_init)
module_exit(sysfs_exit);
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
