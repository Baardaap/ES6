#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <mach/hardware.h>
#include "LPC3250pins.h"
#include "GPIOLPC.h"
#include <asm/io.h>

void setPin(char LPCpin[])
{
}

uint32_t readPin(char LPCpin[])
{
    return 0;
}



void initGPIO()
{
    iowrite32(LCDCFGVALUE,io_p2v(LCD_CFG));
    iowrite32(P0_MUX_SET_VALUE,io_p2v(P0_MUX_CLEAR));
    iowrite32(P1_MUX_SET_VALUE,io_p2v(P1_MUX_SET));
    iowrite32(P2_MUX_SET_VALUE,io_p2v(P2_MUX_SET));
}