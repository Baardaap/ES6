#ifndef LPC3250PINS_H
#define LPC3250PINS_H

typedef struct {
  char LPC[6];
  int  PINS;
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

  #endif 