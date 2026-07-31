#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>

#define CW HIGH
#define CCW LOW

struct Stepper {
  volatile uint8_t *stepPort;
  uint8_t stepBitMask;

  uint8_t dirPin;

  bool enabled;
  bool stepState;

  volatile uint16_t ticks;
  volatile uint16_t counter;

  volatile long pulsecount;
};

extern Stepper x_motor;
extern Stepper y_motor;
extern Stepper conv_motor;

void stepperRamp(Stepper &motor,
                 uint16_t target_speed,
                 bool direction);
void stepperRun(Stepper &motor, int speed, bool direction);
void stepperStop(Stepper &motor);
void stepperUpdate(Stepper &motor);
void stepper_init();

#endif