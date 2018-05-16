#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <mach/hardware.h>
#include "LPC3250pins.h"
#include "GPIOLPC.h"
#include <asm/io.h>
#include <linux/kobject.h>
#include <linux/device.h>

void setPin(char LPCpin[])
{

}

uint32_t readPin(char LPCpin[])
{
    return 0;
}

void initGPIO(void)
{
    iowrite32(LCDCFGVALUE,io_p2v(LCD_CFG));
    iowrite32(P0_MUX_SET_VALUE,io_p2v(P0_MUX_CLEAR));
    iowrite32(P1_MUX_SET_VALUE,io_p2v(P1_MUX_SET));
    iowrite32(P2_MUX_SET_VALUE,io_p2v(P2_MUX_SET));
}

void setDir(void)
{
    iowrite32(0xffff,io_p2v(P2_P3_DIR_SET));
}

void EnableLeds(void)
{
    iowrite32(0xffff,io_p2v(P2_OUT_SET));
}

void DisableLeds(void)
{
    iowrite32(0xffff,io_p2v(P2_OUT_CLEAR));
}

int init_module (void)
{
    initGPIO();
    setDir();
    EnableLeds();
    return 0;
}

void cleanup_module(void)
{
   DisableLeds();
}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Ian en Kong");
MODULE_DESCRIPTION("GPIO");
