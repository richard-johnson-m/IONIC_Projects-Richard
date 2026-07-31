#ifndef DEVICE_H
#define DEVICE_H

typedef enum {
  _roarm = 1,
  _servos = 2,
  _cartesian = 3,
} device_id;

typedef enum {
  on = 1,
  off = 2,
  erratic= 3,
} device_state;

extern device_state state;

extern device_state servo_state;
extern device_state roarm_state;
extern device_state cartesian_state;

void device_control(device_id id, device_state state);
void roarm_status(device_state state);
void servo_control(device_state state);
#endif