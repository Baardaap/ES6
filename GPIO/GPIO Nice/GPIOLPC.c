#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <mach/hardware.h>
#include "LPC3250pins.h"

#define P2_MUX_SET      0x40028028
#define P2_INP_STATE    0x4002801C
#define P2_DIR_SET      0x40028010

#define P2_EMC_D_ENABLE_MASK        0x00000008
#define P2_INPUTDIR_JOYSTICK_MASK   0x0000001F

void setPin(string LPCpin)
{
    return;
}

uint32_t readPin(string LPCpin)
{
    return 0;
}