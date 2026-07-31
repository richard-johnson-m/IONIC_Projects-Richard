#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include<Servo.h>
#include "device.h"
#include<Arduino.h>

extern Servo myservo1;
extern Servo myservo2;
extern Servo myservo3;

void servo_init();
void servo_control(device_state state);

extern uint32_t servo_time;
extern int angle;
extern bool direction;
#endif