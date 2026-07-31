#ifndef DWIN_H
#define DWIN_H
#include <Arduino.h>
#include<SoftwareSerial.h>

extern SoftwareSerial dwinserial;

void display(uint8_t state);
void setPage(uint8_t page);
void setVP(uint16_t address, uint16_t data);
void ir_check();

typedef enum{
  quantity = 0x1001,
  runrate = 0x1002,
  efficiency = 0x1003,

  lapsed_hr = 0x3000,
  lapsed_min = 0x3001,
  lapsed_sec = 0x3002,

  conveyor_icon=0x2702,
  blink_icon =0x2005
}vps;

#endif