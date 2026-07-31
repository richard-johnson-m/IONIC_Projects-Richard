#include "device.h"
#include <Arduino.h>

device_state state;
void device_control(device_id id, device_state state) {
  switch (id) {
    case _servos:
      servo_state=state;
      break;

    case _roarm:
      roarm_state=state;
      break;

    case _cartesian:
      cartesian_state=state;
      break;
  }
}