#include <linux/module.h>
#include <linux/kernel.h>
#include <mach/hardware.h>

#define RTC io_p2v(0x40024000)


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
