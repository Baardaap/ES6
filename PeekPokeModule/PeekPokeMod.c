#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/device.h>
#include <linux/slab.h>

#include <mach/hardware.h>

#include <linux/string.h>

#define sysfs_dir "peekpoke"
#define sysfs_file "data"

#define sysfs_max_data_size 1024

static char sysfs_buffer[sysfs_max_data_size+1] = "No command executed yet\n";
static ssize_t used_buffer_size =0;

//read multiple values from multiple memory adresses
static void 
peek(	uint32_t adress, uint32_t chunks)
		{
			//char* tempbuffer = kmalloc(chunks*10+1, GFP_KERNEL);
			printk("Reading %d chunks from memory %x\n",chunks, adress);
			int i;
			for(i = 0; i < chunks; i++)
			{
				uint32_t *adressptr = (uint32_t*)io_p2v(adress+i);
			    printk("%d\n", *adressptr);
				//char* tempstring = kmalloc(10, GFP_KERNEL);
				//snprintf(tempstring, 10, "%s", *adressptr);
				//strcat(tempbuffer, tempstring);
			}
			//memcpy(sysfs_buffer,tempbuffer, sysfs_max_data_size-1);
			//sysfs_buffer[sysfs_max_data_size] = '\0';

			return;
		}

//write a value to a memory adress
static void
poke(	uint32_t adress,
			uint32_t value)
			{
				printk("Writing value %d to memory %x\n", value, adress);
				uint32_t *adressptr = (uint32_t*)io_p2v(adress);
				*adressptr = value;
				return;
			}

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
				printk(KERN_INFO "Command: %c, Adress: %x, Amount or Value: %u\n",command, adress, amountOrValue);

				switch(command){
					case 'r':	
						peek(adress, amountOrValue);
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

				printk("Used buffer size: %d",used_buffer_size);

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
