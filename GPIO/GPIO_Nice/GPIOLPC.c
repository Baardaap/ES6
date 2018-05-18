#include "GPIOLPC.h"

void setPinDir(int pinNr, char dir, ports_t* port)
{
    char* pin = findPin(pinNr, port);
    int result;
    int PortP = 0;
    int PIndex = 0;
    result = sscanf(pin,"P%i.%i", &PortP, &PIndex);
    switch(PortP)
    {
        case 0:
            if (dir == 'I')
                iowrite32(power(2,PIndex),io_p2v(P0_DIR_CLEAR));
            else
                iowrite32(power(2,PIndex),io_p2v(P0_DIR_SET));
        break;

        case 1:
            if (dir == 'I')
                iowrite32(power(2,PIndex),io_p2v(P1_DIR_CLEAR));
            else
                iowrite32(power(2,PIndex),io_p2v(P1_DIR_SET));
        break;

        case 2:
            if (dir == 'I')
                iowrite32(power(2,PIndex),io_p2v(P2_P3_DIR_CLEAR));
            else
                iowrite32(power(2,PIndex),io_p2v(P2_P3_DIR_SET));
        break;

        case 3:
            if (dir == 'I')
                iowrite32(power(2,PIndex),io_p2v(P2_P3_DIR_CLEAR));
            else
                iowrite32(power(2,PIndex),io_p2v(P2_P3_DIR_SET));
        break; 
    }      
}

void setPinOut(int pinNr, char output, ports_t* port)
{
    char* pin = findPin(pinNr, port);
    int result;
    int PortP = 0;
    int PIndex = 0;
    result = sscanf(pin,"P%i.%i", &PortP, &PIndex);
    switch(PortP)
    {
        case 0:
            if (output == 'H')
                iowrite32(power(2,PIndex),io_p2v(P0_OUT_SET));
            else
                iowrite32(power(2,PIndex),io_p2v(P0_OUT_CLEAR));
        break;

        case 1:
            if (output == 'H')
                iowrite32(power(2,PIndex),io_p2v(P1_OUT_SET));
            else
                iowrite32(power(2,PIndex),io_p2v(P1_OUT_CLEAR));
        break;

        case 2:
            if (output == 'H')
                iowrite32(power(2,PIndex),io_p2v(P2_OUT_SET));
            else
                iowrite32(power(2,PIndex),io_p2v(P2_OUT_CLEAR));
        break;

        case 3:
            if (output == 'H')
                iowrite32(power(2,PIndex),io_p2v(P3_OUT_SET));
            else
                iowrite32(power(2,PIndex),io_p2v(P3_OUT_CLEAR));
        break;    
    }
}

uint32_t readPin(int pinNr, ports_t* port)
{
    char* pin = findPin(pinNr, port);
    int result;
    int PortP = 0;
    int PIndex = 0;
    result = sscanf(pin,"P%i.%i", &PortP, &PIndex);
    switch(PortP)
    {
        case 0:
            return ioread32(io_p2v(P0_INP_STATE));
        break;

        case 1:
            return ioread32(io_p2v(P1_INP_STATE));
        break;

        case 2:
            return ioread32(io_p2v(P2_INP_STATE));
        break;

        case 3:
            return ioread32(io_p2v(P3_INP_STATE));
        break;    
    }
    return -1;
}

char* findPin(int PinNumber, ports_t* Port)
{
    int i;
    for (i = 0; i < (sizeof(Port)/sizeof(Port[0])); i++)
    {
        if (Port[i].PINS == PinNumber)
        {
            return Port[i].LPC;
        } 
    }
    return "";
}

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

void initGPIO()
{
    iowrite32(LCDCFGVALUE,io_p2v(LCD_CFG));
    iowrite32(P0_MUX_SET_VALUE,io_p2v(P0_MUX_CLEAR));
    iowrite32(P1_MUX_SET_VALUE,io_p2v(P1_MUX_SET));
    iowrite32(P2_MUX_SET_VALUE,io_p2v(P2_MUX_SET));
}