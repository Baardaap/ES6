#include <linux/module.h>
#include <linux/kernel.h>
#include <mach/hardware.h>
//#include <stdio.h>

#define RTC0 io_p2v(0x40024000)
#define RTC1 io_p2v(0x40024001)
#define RTC2 io_p2v(0x40024002)
#define RTC3 io_p2v(0x40024003)

int init_module(void)
{
	int val;
    uint32_t *a = (uint8_t*)  RTC0;
    val = *a;
    printk("%d\n", val);
	return 0;
}

void cleanup_module(void)
{
	int val;
    uint32_t *a = (uint32_t*) RTC0;
    val = *a;
    printk("%d\n", val);
}