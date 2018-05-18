#ifndef GPIOLPC_H
#define GPIOLPC_H

#include "LPC3250pins.h"

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

void initGPIO(void);
void setPinDir(int pinNr, char dir, ports_t* port);
void setPinOut(int pinNr, char output, ports_t* port);
uint32_t readPin(int pinNr, ports_t* port);
char* findPin(int PinNumber, ports_t* Port);
int  power(int value, int index); 

#endif