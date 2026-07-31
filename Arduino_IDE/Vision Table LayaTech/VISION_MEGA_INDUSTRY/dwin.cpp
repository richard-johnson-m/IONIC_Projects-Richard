#include<Arduino.h>
#include"dwin.h"
#include "cartesian.h"

vps disp_vp;
uint16_t vp_addr, data;
uint32_t update_time = 0;
uint32_t ir_count = 0;

#define obj_ir_pin 22
#define end_ir_pin 23
void display(uint8_t state)
{
  static uint32_t elapsed_seconds = 0;
  static uint32_t previous_time = 0;

  static uint32_t count = 0;
  static uint32_t blink_time = 0;
  static uint8_t blinking = 3;
  static uint16_t i = 1;

  if (cartesian_state == 1)  //normall
  {
    setVP(conveyor_icon, 0);
    setVP(blink_icon, 2);                       
    setVP(quantity, 500);

    setVP(efficiency, 100);

    setVP(runrate, ir_count);

  }

  else if (cartesian_state == 3) //eroor
  {
    setVP(conveyor_icon, 1);

    setVP(quantity, 500);
    setVP(runrate, 0);
    setVP(efficiency, 0);

    if (millis() - blink_time >= 500)
    {
      setVP(blink_icon, blinking);
      blinking = (blinking == 3) ? 2 : 3;
      blink_time = millis();
    }
  }

  if (state == 1 || state == 2)
  {
    if (millis() - previous_time >= 1000)
    {
      previous_time = millis();
      elapsed_seconds++;

      uint16_t hr = elapsed_seconds / 3600;
      uint16_t min = (elapsed_seconds % 3600) / 60;
      uint16_t sec = elapsed_seconds % 60;

      if (hr > 99)
      {
        elapsed_seconds = 0;
        hr = 0;
        min = 0;
        sec = 0;
      }

      setVP(lapsed_hr, hr);
      setVP(lapsed_min, min);
      setVP(lapsed_sec, sec);
    }
  }
}

void setPage(uint8_t page) {
  //5A A5 07 82 00 84 5a 01 00 02
  uint8_t sendBuffer[] = { 0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, page };
  dwinserial.write(sendBuffer, sizeof(sendBuffer));
}
void setVP(uint16_t address, uint16_t data) {
  uint8_t sendBuffer[] = {
    0x5A,
    0xA5,
    0x05,
    0x82,
    (address >> 8) & 0xFF,
    address & 0xFF,
    (data >> 8) & 0xFF,
    data & 0xFF
  };
  dwinserial.write(sendBuffer, sizeof(sendBuffer));
}

void ir_check()
{
    static bool prev_ir = 0;

    bool ir = digitalRead(obj_ir_pin);

    if (ir == 1 && prev_ir == 0)
    {
        ir_count++;
        waited=false;
        // Serial.print("Count = ");
        // Serial.println(count);
    }

    prev_ir = ir;
}