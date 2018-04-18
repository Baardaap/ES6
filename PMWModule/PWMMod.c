#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <mach/hardware.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Konghon Choo & Ian Seelen");
MODULE_DESCRIPTION("A PWM driver for the LPC3250");

#define NAME                "PWMMod"

#define PWMID1              1
#define PWMID2              2
#define DIV_HIGH_FREQ       1
#define DIV_LOW_FREQ        15

#define PWMCLOCK_REG		0x400040B8
#define PWMCLOCK_VAL		0x115
#define LCDCLOCK_REG		0x40004054
#define LCDCLOCK_VAL		0

#define PWM1_REG            0x4005C000
#define PWM2_REG    		0x4005C004

#define REL_SHIFT           8
#define PWM1_DIV_SHIFT      4
#define PWM2_DIV_SHIFT      8

#define PWM_ENABLE_MASK     0x80000000
#define DIV_CLEAR_MASK      0x0f
#define CLEAR_MASK          0xff

#define BUFFER_LENGTH       80

#define BASE_FREQ           32768

int deviceOpened = 0;
char message[BUFFER_LENGTH];
char *messagePtr;

int major;


/* uint8_t calculateReloadValue(float freq, uint8_t division)
{
    uint8_t reloadValue = (BASE_FREQ / division)/(256 * freq);
    if(reloadValue >= 256)
    {
        reloadValue = 0;
    }
    return reloadValue;
} */

uint8_t calculateDutyValue(uint8_t duty) 
{
	return (duty * 100) / 256;
}

uint32_t getPWMRegister(uint8_t pwmID)
{
    if(pwmID == 1)
    {
        return PWM1_REG;
    }
    else if(pwmID == 2)
    {
        return PWM2_REG;
    }
    else
    {
        printk(KERN_INFO "Error: Unknown PWM ID\n");
        return -1;
    }
}

void enablePWM(uint8_t value, uint8_t pwmID)
{
    uint32_t writeRegister = getPWMRegister(pwmID);
    if(value == 1)
    {
        *(uint32_t*)(io_p2v(writeRegister)) &= ~PWM_ENABLE_MASK;
        *(uint32_t*)(io_p2v(writeRegister)) |= PWM_ENABLE_MASK; 
    }
    else if(value == 0)
    {
        *(uint32_t*)(io_p2v(writeRegister)) &= ~PWM_ENABLE_MASK;
        *(uint32_t*)(io_p2v(writeRegister)) |= PWM_ENABLE_MASK;
    }
    else
    {
        printk(KERN_INFO "Error: Wrong Input\n");
    }
}

void setPWM_DutyCycle(uint8_t duty, uint8_t pwmID)
{
    if(duty < 1 || duty > 100)
    {
       printk(KERN_INFO "Error: Wrong Input\n");
    }
    else
    {
        uint32_t writeRegister = getPWMRegister(pwmID);

        *(uint32_t*)(io_p2v(writeRegister)) &= ~CLEAR_MASK;
        *(uint32_t*)(io_p2v(writeRegister)) |= calculateDutyValue(duty);
    }
}

void setPWM_ReloadValue(uint8_t rel, uint8_t pwmID, uint8_t division)
{
    uint32_t writeRegister = getPWMRegister(pwmID);
    uint32_t clearMask = CLEAR_MASK << REL_SHIFT;
    uint32_t writeMask = rel << REL_SHIFT;

    *(uint32_t*)(io_p2v(writeRegister)) &= ~clearMask;
    *(uint32_t*)(io_p2v(writeRegister)) |= writeMask;
}

uint8_t setPWM_Div(uint8_t division, uint8_t pwmID)
{
    uint8_t bitsToShift = 0;
    if(pwmID == 1)
    {
        bitsToShift = PWM1_DIV_SHIFT;
    }
    else if(pwmID == 2)
    {
        bitsToShift = PWM2_DIV_SHIFT;
    }
    else
    {
        printk(KERN_INFO "Error: Unknown PWM ID\n");
        return -1;
    }
    uint32_t clearMask = (uint32_t)DIV_CLEAR_MASK << bitsToShift;
    *(uint32_t*)(io_p2v(PWMCLOCK_REG)) &= clearMask;

    uint32_t setMask = (uint32_t)division << bitsToShift;
    *(uint32_t*)(io_p2v(PWMCLOCK_REG)) |= setMask;
    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
    ssize_t bytes_read = 0;
    return bytes_read;
}

static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t *off)
{
    uint8_t minor = (uint32_t)filp->private_data;
    int writeValue = -1;

    uint8_t i;
    for(i=0; i < len && i < BUFFER_LENGTH; i++)
    {
        get_user(message[i], buff + i);
    }

    messagePtr = message;
    sscanf(messagePtr, "%d", &writeValue);

    switch(minor)
    {
        case 0: //Enable PWM1
            enablePWM(writeValue, PWMID1);
            break;
        case 1: //Enable PWM2
            enablePWM(writeValue, PWMID2);
            break;
        case 2: //PWM 1 High Frequency
            setPWM_Div(DIV_HIGH_FREQ, PWMID1);
            setPWM_ReloadValue(writeValue, PWMID1, DIV_HIGH_FREQ);
            break;
        case 3: //PWM 2 High Frequency
            setPWM_Div(DIV_HIGH_FREQ, PWMID2);
            setPWM_ReloadValue(writeValue, PWMID2, DIV_HIGH_FREQ);
            break;
        case 4: //PWM 1 Duty Cycle
            setPWM_DutyCycle(writeValue, PWMID1);
            break;
        case 5: //PWM 2 Duty Cycle
            setPWM_DutyCycle(writeValue, PWMID2);
            break;
        case 6: //PWM 1 Low Frequency
            setPWM_Div(DIV_LOW_FREQ, PWMID1);
            break;
        case 7: //PWM 2 Low Frequency
            setPWM_Div(DIV_LOW_FREQ, PWMID2);
            break;
        default:
            printk(KERN_INFO "Error: Written to wrong file\n");
            break;
    }
    return writeValue;
}

static int device_open(struct inode *inode, struct file *file)
{
    int minor = MINOR(inode->i_rdev);

	messagePtr = message;
	
	file->private_data = (void*)minor;

	if (deviceOpened) return -EBUSY;
	deviceOpened++;

	printk(KERN_INFO "Major: %d, minor: %d\n", MAJOR(inode->i_rdev), minor);

	return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    deviceOpened--;
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
        printk(KERN_INFO "Registering device failed, Error: %d\n", major);
        return major;
    }
    printk(KERN_INFO "Registering device %s on major: %d succeeded \
    Please use 'mknod' to create the appropriate files\n", NAME, major); 

    *(uint32_t*)(io_p2v(PWMCLOCK_REG)) = PWMCLOCK_VAL;
    *(uint32_t*)(io_p2v(LCDCLOCK_REG)) = LCDCLOCK_VAL;

    return 0;
}

void cleanup_module(void)
{
    unregister_chrdev(major, NAME);
}