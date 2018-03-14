#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/device.h>

#include <mach/hardware.h>

#include <linux/string.h>

#define sysfs_dir "peekpoke"
#define sysfs_file "data"

#define sysfs_max_data_size 1024

//read a value from a memory adress 
static uint32_t
readFromMem(	uint32_t adress)
			{
				printk(KERN_INFO "Method readFromMem\n");
			    uint32_t *adressptr = (uint32_t*)io_p2v(adress);
			    printk(" reading from memory %d\n", *adressptr);
			    return *adressptr;
			}

//read multiple values from multiple memory adresses
static void 
peek(	uint32_t initAdress, uint32_t chunks, char *buffer)
		{
			printk(KERN_INFO "Method peek\n");
			
			int i;
			for(i = 0; i <= chunks; i++)
			{
				strcat((char*)buffer, (char*)readFromMem(initAdress+i));
			}
			return;
		}

//write a value to a memory adress
static void
poke(	uint32_t adress,
			uint32_t value)
			{
				printk(KERN_INFO "Method poke\n");
				uint32_t *adressptr = (uint32_t*)io_p2v(adress);
				*adressptr = value;
				return;
			}

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
				printk(KERN_INFO "sysfile_write (/sys/kernel/%s/%s) called\n",sysfs_dir,sysfs_file);

				char command = ' ';
				uint32_t adress = 0;
				uint32_t amountOrValue = 0;
				sscanf(buffer, "%c %x %u", &command, &adress, &amountOrValue);

				switch(command){
					case 'r':
						printk(KERN_INFO "Command: %c, Adress: %u, Amount or Value: %u\n",command, adress, amountOrValue);
						peek(adress, amountOrValue, (char*)buffer);
					break;

					case 'w':
						poke(adress, amountOrValue);
					break;

					default:
						printk(KERN_INFO "Command: %c is not a valid command. Try 'r' or 'w'.\n", command);
					break;
				}
				
				used_buffer_size = count > sysfs_max_data_size ? sysfs_max_data_size :count;
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
