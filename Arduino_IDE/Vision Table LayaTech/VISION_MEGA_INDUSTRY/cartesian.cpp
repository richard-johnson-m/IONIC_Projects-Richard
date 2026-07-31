#include "cartesian.h"
#include <Arduino.h>
#include "color.h"
#include "dwin.h"
extern device_state state;
#define obj_ir_pin 22
#define end_ir_pin 23
#define status_pin 30

bool waited = false;

uint16_t red_count = 0;
uint16_t green_count = 0;
void cartesian_control(device_state state) {

  static uint32_t prev = 0;

  uint8_t first_ir = digitalRead(obj_ir_pin);
  uint8_t second_ir = digitalRead(end_ir_pin);

  // Serial.print("IR1 ");
  // Serial.print(first_ir);
  // Serial.print("   IR2 ");
  // Serial.println(second_ir);

  static bool page_active = false;
  static unsigned long page_start = 0;
  const char* color = color_read();  // FIXED

  // const char* color='R';

  static unsigned long color_detect_time = 0;
  static uint32_t color_detect_start = 0;
  static bool detecting = false;
  static bool sampling_started = false;

    // Serial.print("   RED : ");
    // Serial.print(red_count);
    // Serial.print("   green : ");
    // Serial.print(green_count);
    // Serial.print("    Color = ");
    // Serial.println(color);
    // Start detection when object is detected (example using IR)
    if (first_ir == 1 && detecting == false && page_active == false) {
    detecting = true;

    color_detect_start = millis();  // travel delay timer

    red_count = 0;
    green_count = 0;

    sampling_started = false;
  }

  if (detecting) {

    // Wait 500 ms for object to reach sensor
    if (!sampling_started) {

      if (millis() - color_detect_start >= 1000) {
        sampling_started = true;
        color_detect_time = millis();  // start sampling timer
      }

    } else {

      // Sample color for 500 ms
      if (millis() - color_detect_time <= 500) {

        if (color[0] == 'R') {
          red_count++;
        } else if (color[0] == 'G') {
          green_count++;
        }

      } else {

        if (red_count > green_count) {
          setPage(2);
        } else {
          setPage(1);
        }

        page_active = true;
        page_start = millis();

        detecting = false;
        sampling_started = false;
      }
    }
  }

  if (page_active && (millis() - page_start >= 1000)) {
    setPage(0);
    page_active = false;
  }
  // UART data sending
  if (millis() - prev >= 100) {
    prev = millis();

    Serial3.print(state);
    Serial3.print(",");
    Serial3.print(first_ir);
    Serial3.print(",");
    Serial3.print(second_ir);
    Serial3.print(",");
    Serial3.println(color);
  }

  ///tower lmap logic
  if (state == on) {
    digitalWrite(cartesian_tower, LOW);
  } else if (state == erratic) {
    digitalWrite(cartesian_tower, HIGH);
  }
}
