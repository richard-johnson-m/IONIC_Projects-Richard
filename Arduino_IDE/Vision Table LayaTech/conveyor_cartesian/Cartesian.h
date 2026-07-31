#ifndef CARTESIAN_H
#define CARTESIAN_H

#include<Arduino.h>

typedef enum{
  home,
  conveyor_run,
  object_color_detection,
  object_destiny,
  pick_and_place,
  stop,
  reset_flag,
  reset
}state_machine;
extern state_machine state;

typedef enum{
  y_pick_down,
  y_pick_up,
  x_goes_right,
  y_place_down,
}cartesian_state;
extern cartesian_state cart_state;

extern bool x_limit_reached;
extern bool y_limit_reached;

extern uint32_t x_step_pulse;
void cartesian_process();
void cartesian_off();
extern uint8_t hold_magnet;
#endif