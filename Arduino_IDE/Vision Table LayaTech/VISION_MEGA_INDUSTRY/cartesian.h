#ifndef CARTESIAN_H
#define CARTESIAN_H
#include "device.h"
#include <Arduino.h> 

#define cartesian_tower 7
extern uint16_t red_count;
extern uint16_t green_count;
extern bool waited;
void cartesian_control(device_state state);


#endif