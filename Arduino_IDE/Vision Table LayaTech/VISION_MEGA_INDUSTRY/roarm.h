#ifndef ROARM_H
#define ROARM_H

#include<Arduino.h>
#include "device.h"

#define roarm_tower 6
#define ground_relay 5

extern device_state state;
unsigned long lastMove = 0;
unsigned long lastTime = 0;

int interval = 150;     

int stepIndex = 0;
void roarm_status(device_state state);

#endif