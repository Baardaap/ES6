#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <mach/hardware.h>
#include <linux/kobject.h>
#include <linux/device.h>

#define sysfs_dir  "buffer"
#define sysfs_file "data"

#define sysfs_max_data_size 1024 
static char sysfs_buffer[sysfs_max_data_size+1] = "HelloWorld!\n"; 
static ssize_t used_buffer_size = 0;

#define DEVICE_NAME "GPIOnode"
#define MAJORNUMBER 99 

#define LCDCFGVALUE 0x0
#define LCD_CFG 0x40004054

#define P0_MUX_CLEAR 0x40028124
#define P0_DIR_CLEAR 0x40028054
#define P0_INP_STATE 0x40028040
#define P0_DIR_SET 0x40028050
#define P0_OUT_SET  0x40028044
#define P0_OUT_CLEAR 0x40028048

#define P1_MUX_SET 0x40028130
#define P1_DIR_CLEAR 0x40028074
#define P1_INP_STATE 0x40028060
#define P1_DIR_SET 0x40028070
#define P1_OUT_SET  0x40028064
#define P1_OUT_CLEAR 0x40028068

#define P2_MUX_SET 0x40028028
#define P2_INP_STATE 0x4002801C
#define P2_OUT_SET  0x40028020
#define P2_OUT_CLEAR 0x40028024

#define P2_P3_DIR_CLEAR 0x40028014
#define P2_P3_DIR_SET 0x40028010

#define P3_INDEX 25

#define P3_MUX_SET 0x40028110
#define P3_INP_STATE 0x40028000
#define P3_OUT_SET  0x40028004
#define P3_OUT_CLEAR 0x40028008

#define P0_MUX_SET_VALUE 0
#define P1_MUX_SET_VALUE 0xFF8000
#define P2_MUX_SET_VALUE 8

#define SET_PORT 0
#define WRITE_PORT 1
#define SET_O 0
#define SET_I 1
#define LOW 0
#define HIGH 1

#define READSHIFTVALUE 31


static int Device_Open = 0;
char* data;
int echoJumper = -1;
int echoIndexPin = -1;

  typedef struct {
    char LPC[5];
    int  QVGA;
  } ports_t;

  ports_t J1list[] = {
    {"P2.8",49},
    {"P2.9",50},
    {"P2.10",51},
    {"P2.11",52},
    {"P2.12",53},
    {"P0.7",27},
    {"P3.5",24},
    {"P1.16",44},
    {"P1.17",43},
    {"P1.18",42},
    {"P1.19",41},
    {"P1.20",40},
    {"P1.21",39},
    {"P1.22",38},
    {"P1.23",37}};
  ports_t J2list[] = {
    {"P0.1",24},
    {"P0.2",11},
    {"P0.3",12},
    {"P0.4",13},
    {"P0.5",14},
    {"P1.0",42},
    {"P1.15",27}};

  ports_t J3list[] = {
    {"P2.0",47},
    {"P2.1",56},
    {"P2.2",48},
    {"P2.3",57},
    {"P2.4",49},
    {"P2.5",58},
    {"P2.6",50},
    {"P2.7",45},
    {"P0.0",40},
    {"P0.6",33},
    {"P3.0",54},
    {"P3.1",46},
    {"P3.4",36}};

int dir_clear_arr[] = {P0_DIR_CLEAR,P1_DIR_CLEAR,P2_P3_DIR_CLEAR,P2_P3_DIR_CLEAR};
int dir_set_arr[] = {P0_DIR_SET,P1_DIR_SET,P2_P3_DIR_SET,P2_P3_DIR_SET};
int outp_clear_arr[] = {P0_OUT_CLEAR,P1_OUT_CLEAR,P2_OUT_CLEAR,P3_OUT_CLEAR};
int outp_set_arr[] = {P0_OUT_SET,P1_OUT_SET,P2_OUT_SET,P3_OUT_SET};
int inp_state_arr[] = {P0_INP_STATE,P1_INP_STATE,P2_INP_STATE,P3_INP_STATE};
int inp_state_p3[] = {10,11,12,13,14,24};


int power(int value, int index) 
{ 
    int i = 0;
    int result = 1;
    if (index == 0) return 1;
    for (i = 0; i < index; i++)
        {
            result = result * value;
        }
    return result;
}

int writeIO(char LPCaddress[], int io, int setOrWrite)
{
    int result;
    int PortP = 0;
    int PIndex = 0;
    result = sscanf(LPCaddress,"P%i.%i", &PortP, &PIndex);
    if (PortP == 3)
        PIndex = PIndex + P3_INDEX;

    switch (setOrWrite)
    {
        case 0:

            if (io == 0)
                iowrite32(power(2,PIndex),io_p2v(dir_set_arr[PortP]));
            else if (io == 1)
                iowrite32(power(2,PIndex),io_p2v(dir_clear_arr[PortP]));
            else 
            {
                printk(KERN_INFO "Input error! -> enter I or O");
                return -1;
            }
        break;

        case 1:
            if (io == 0)
                iowrite32(power(2,PIndex),io_p2v(outp_clear_arr[PortP]));
            else if (io == 1)
                iowrite32(power(2,PIndex),io_p2v(outp_set_arr[PortP]));
            else 
            {
                printk(KERN_INFO "Input error! -> enter H or L");
                return -1;
            }
        break;

        default:
        printk(KERN_INFO "Input error!");
        break;

    }
     return 0;
    
}

int readIO(char LPCaddress[])
{

    int result;
    int PortP = 0;
    int PIndex = 0;
    unsigned int readvalue;
    result = sscanf(LPCaddress,"P%i.%i", &PortP, &PIndex);
    if(PortP == 3)
        PIndex = inp_state_p3[PIndex];

    readvalue = ioread32(io_p2v(inp_state_arr[PortP]));
    readvalue = readvalue << (READSHIFTVALUE - PIndex);
    readvalue = readvalue >> (READSHIFTVALUE);
    printk( "%zu" ,readvalue);
    return readvalue;
    
}


int findAndSetPin(int jumper, int indexpin, int setOrWrite, int value)
{
    int i;
    switch (jumper) 
        {
            case 1:
                for (i = 0; i < (sizeof(J1list) / sizeof(J1list[0])); i++)
                {
                    if (J1list[i].QVGA == indexpin)
                    {
                            return writeIO(J1list[i].LPC, value, setOrWrite);
                    } 
                }
                printk(KERN_INFO "Geen GPIO pin gevonden");
                break;
            case 2:
                for (i = 0; i < (sizeof(J2list) / sizeof(J2list[0])); i++)
                {
                    if (J2list[i].QVGA == indexpin)
                    {
                            return  writeIO(J2list[i].LPC, value, setOrWrite);
                    }
                }
                printk(KERN_INFO "Geen GPIO pin gevonden");
                break;
            case 3:
                for (i = 0; i < (sizeof(J3list) / sizeof(J3list[0])); i++)
                {
                    if (J3list[i].QVGA == indexpin)
                    {
                            return writeIO(J3list[i].LPC, value, setOrWrite); 
                    }
                }
                printk(KERN_INFO "Geen GPIO pin gevonden");
                break;
            default:
                printk(KERN_INFO "No such jumper number.");
                break;
        }
        return -1;
}

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
    if(echoJumper == -1 || echoIndexPin ==  -1)
    {
        printk(KERN_INFO "Echo R first!");
        return 0;
    }
    else
    {
        switch(echoJumper)
        {
            case 1:
                for (i = 0; i < (sizeof(J1list) / sizeof(J1list[0])); i++)
                    {
                        if (J1list[i].QVGA == echoIndexPin)
                        {
                                result = readIO(J1list[i].LPC);
                                return 0;
                        }
                    }
                    printk(KERN_INFO "Geen GPIO pin gevonden");
                break;

            case 2:
                for (i = 0; i < (sizeof(J2list) / sizeof(J2list[0])); i++)
                    {
                        if (J2list[i].QVGA == echoIndexPin)
                        {
                                result = readIO(J2list[i].LPC);
                                return 0;
                        }
                    }
                    printk(KERN_INFO "Geen GPIO pin gevonden");
                break;

            case 3:
                for (i = 0; i < (sizeof(J3list) / sizeof(J3list[0])); i++)
                    {
                        if (J3list[i].QVGA == echoIndexPin)
                        {
                                result = readIO(J3list[i].LPC);
                                return 0;
                        }
                    }
                    printk(KERN_INFO "Geen GPIO pin gevonden");
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
        if(minornumber == 0) 
        {   
            if(value == 'H')
                result = findAndSetPin(jumper,indexpin,WRITE_PORT,HIGH);
            else if (value == 'L')
                result = findAndSetPin(jumper,indexpin,WRITE_PORT,LOW);
            else if (value == 'R')
            {
                echoJumper = jumper;
                echoIndexPin = indexpin;
            }
            else 
                printk(KERN_INFO "Input error! -> enter H (high) L (low) R (read)");
        }
        
    }
    return length;
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
    int result = 0;
    int majorNr = register_chrdev (MAJORNUMBER, DEVICE_NAME, &Fops);
    hello_obj = kobject_create_and_add(sysfs_dir, kernel_kobj);
    if (hello_obj == NULL)
    {
        printk (KERN_INFO "%s module failed to load: kobject_create_and_add failed\n", sysfs_file);
        return -ENOMEM;
    }

    result = sysfs_create_group(hello_obj, &attr_group);
    if (result != 0)
    {
        printk (KERN_INFO "%s module failed to load: sysfs_create_group failed with result %d\n", sysfs_file, result);
        kobject_put(hello_obj);
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

    iowrite32(LCDCFGVALUE,io_p2v(LCD_CFG));
    iowrite32(P0_MUX_SET_VALUE,io_p2v(P0_MUX_CLEAR));
    iowrite32(P1_MUX_SET_VALUE,io_p2v(P1_MUX_SET));
    iowrite32(P2_MUX_SET_VALUE,io_p2v(P2_MUX_SET));
    return result;
}

void cleanup_module(void)
{
    unregister_chrdev(MAJORNUMBER, DEVICE_NAME);
    kobject_put(hello_obj);
    printk (KERN_INFO "/sys/kernel/%s/%s removed\n", sysfs_dir, sysfs_file);
}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Koen en Rens");
MODULE_DESCRIPTION("GPIO");


