#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <mach/hardware.h>
#include <mach/platform.h>
#include <mach/irqs.h>
#include <asm/uaccess.h>

#define DEVICE_NAME 		"adc"
#define ADC_NUMCHANNELS		3

// adc registers
#define	ADCLK_CTRL			io_p2v(0x400040B4)
#define	ADCLK_CTRL1			io_p2v(0x40004060)
#define	ADC_SELECT			io_p2v(0x40048004)
#define	ADC_CTRL			io_p2v(0x40048008)
#define ADC_VALUE           io_p2v(0x40048048)
#define SIC1_ER             io_p2v(0x4000C000) //Enable Register
#define AD_PDN_CTRL         1 << 2
#define ADC_INT             1 << 7 
#define ADC_READ_MASK       0x1FF
#define AD_STROBE           1 << 1

#define ADC_RTC             0x1
#define ADC_PERIPH_CLK      0x01ff
#define ADC_POS_NEG_REF1    0x03c0
#define ADC_POS_NEG_REF2    0x0280
#define ADC_CHANNEL_BIT     0x0030



#define READ_REG(a)         (*(volatile unsigned int *)(a))
#define WRITE_REG(b,a)      (*(volatile unsigned int *)(a) = (b))

//GPIO REG
#define SIC2_ATR            io_p2v(0x40010010)
#define GPI01               1 << 23

static unsigned char    adc_channel = 0;
static int              adc_values[ADC_NUMCHANNELS] = {0, 0, 0};


static irqreturn_t      adc_interrupt (int irq, void * dev_id);
static irqreturn_t      gp_interrupt (int irq, void * dev_id);

DECLARE_WAIT_QUEUE_HEAD(ADC_Ready_q);
bool ADCIsReady = false;

static void adc_init (void)
{
	unsigned long data;

	// set 32 KHz RTC clock
    data = READ_REG (ADCLK_CTRL);
    data |= ADC_RTC;
    WRITE_REG (data, ADCLK_CTRL);
    
	// rtc clock ADC & Display = from PERIPH_CLK
    data = READ_REG (ADCLK_CTRL1);
    data &= ~ADC_PERIPH_CLK;
    WRITE_REG (data, ADCLK_CTRL1);

	// negatief & positieve referentie
    data = READ_REG(ADC_SELECT);
    data &= ~ADC_POS_NEG_REF1;
    data |=  ADC_POS_NEG_REF2;
    WRITE_REG (data, ADC_SELECT);

    //Set GPI01 Flank Detection
    data = READ_REG (SIC2_ATR);
    data |= GPI01;
    WRITE_REG (data, SIC2_ATR);

	// Enable ADC Interrupt
    data = READ_REG(SIC1_ER);
    data |= ADC_INT;
    WRITE_REG (data, SIC1_ER);

    //aanzetten adc en reset
    data = READ_REG(ADC_CTRL);
    data |= AD_PDN_CTRL;
    WRITE_REG (data, ADC_CTRL);

	//IRQ init
    if (request_irq (IRQ_LPC32XX_TS_IRQ, adc_interrupt, IRQF_DISABLED, "", NULL) != 0)
    {
        printk(KERN_ALERT "ADC IRQ request failed\n");
    }

    if (request_irq (IRQ_LPC32XX_GPI_01, gp_interrupt, IRQF_DISABLED, "", NULL) != 0)
    {
        printk (KERN_ALERT "GP IRQ request failed\n");
    }
}


static void adc_start (unsigned char channel)
{
	unsigned long data;

	if (channel >= ADC_NUMCHANNELS)
    {
        channel = 0;
    }

	data = READ_REG (ADC_SELECT);

	//selecteer het kanaal, eerst uitlezen, kanaalbits negeren en dan alleen de kanaalbits veranderen (0x0030)
	WRITE_REG((data & ~ADC_CHANNEL_BIT) | ((channel << 4) & ADC_CHANNEL_BIT), ADC_SELECT);

	//nu ook globaal zetten zodat we de interrupt kunnen herkennen
	adc_channel = channel;

	// start conversie
    data = READ_REG(ADC_CTRL);
    data |= AD_STROBE;
    WRITE_REG (data, ADC_CTRL);
}

static irqreturn_t adc_interrupt (int irq, void * dev_id)
{
    adc_values[adc_channel] = READ_REG(ADC_VALUE);
    adc_values[adc_channel] &= ADC_READ_MASK;

    ADCIsReady =true;
    wake_up_interruptible(&ADC_Ready_q);
    return (IRQ_HANDLED);
}

static irqreturn_t gp_interrupt(int irq, void * dev_id)
{
    adc_start (0);
    printk (KERN_INFO DEVICE_NAME "ButtonPressed");
    return (IRQ_HANDLED);
}


static void adc_exit (void)
{
    free_irq (IRQ_LPC32XX_TS_IRQ, NULL);
    free_irq (IRQ_LPC32XX_GPI_01, NULL);
}


static ssize_t device_read (struct file * file, char __user * buf, size_t length, loff_t * f_pos)
{
	int     channel = (int) file->private_data;
    char	return_buffer[128];
    int     bytes_read = 0;
    int     bytesWritten;
    int     bytesToWrite;

    if (*f_pos > 0) {
		*f_pos = 0;
		return 0;
	}

    printk (KERN_WARNING DEVICE_NAME ":device_read(%d)\n", channel);

    if (channel < 0 || channel >= ADC_NUMCHANNELS)
    {
		return -EFAULT;
    }

    adc_start (channel);

    ADCIsReady = false;
    wait_event_interruptible(ADC_Ready_q,(ADCIsReady == true));

    // TODO: wait for end-of-conversion,
    // read adc and copy it into 'buf'

    bytesWritten = sprintf(return_buffer, "%d\n", adc_values[adc_channel]);
	bytesToWrite = copy_to_user(buf, return_buffer, bytesWritten);

    *f_pos = bytesWritten;
	return (bytesWritten);

    return (bytes_read);
}




static int device_open (struct inode * inode, struct file * file)
{
    // get channel from 'inode'
    int minor = MINOR(inode->i_rdev);
    file->private_data = minor;

    try_module_get(THIS_MODULE);
    return 0;
}


static int device_release (struct inode * inode, struct file * file)
{
    //printk (KERN_WARNING DEVICE_NAME ": device_release()\n");


    module_put(THIS_MODULE);
	return 0;
}


static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .read = device_read,
    .open = device_open,
    .release = device_release
};


static struct chardev
{
    dev_t       dev;
    struct cdev cdev;
} adcdev;


int adcdev_init (void)
{
    // try to get a dynamically allocated major number
	int error = alloc_chrdev_region(&adcdev.dev, 0, ADC_NUMCHANNELS, DEVICE_NAME);;

	if(error < 0)
	{
		// failed to get major number for our device.
		printk(KERN_WARNING DEVICE_NAME ": dynamic allocation of major number failed, error=%d\n", error);
		return error;
	}

	printk(KERN_INFO DEVICE_NAME ": major number=%d\n", MAJOR(adcdev.dev));

	cdev_init(&adcdev.cdev, &fops);
	adcdev.cdev.owner = THIS_MODULE;
	adcdev.cdev.ops = &fops;

	error = cdev_add(&adcdev.cdev, adcdev.dev, ADC_NUMCHANNELS);
	if(error < 0)
	{
		// failed to add our character device to the system
		printk(KERN_WARNING DEVICE_NAME ": unable to add device, error=%d\n", error);
		return error;
	}

    // set EINT0 button to activate on edge
    unsigned long data;
    data = READ_REG (SIC2_ATR);
    data |= 0x400000;
    WRITE_REG (data, SIC2_ATR);
    
	adc_init();

	return 0;
}


/*
 * Cleanup - unregister the appropriate file from /dev
 */
void cleanup_module()
{
	cdev_del(&adcdev.cdev);
	unregister_chrdev_region(adcdev.dev, ADC_NUMCHANNELS);

	adc_exit();
}


module_init(adcdev_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joris Geurts, Tobias Neyssen & Leon Leijssen");
MODULE_DESCRIPTION("ADC Driver");
