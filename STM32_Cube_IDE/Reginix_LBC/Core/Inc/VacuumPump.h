/*
 * VacuumPump.h
 *
 *  Created on: 22-Mar-2026
 *      Author: tamil
 */

#ifndef INC_VACUUMPUMP_H_
#define INC_VACUUMPUMP_H_


#include "stm32f1xx_hal.h"

class VacuumPump
{
public:
    VacuumPump(TIM_HandleTypeDef *htim, uint32_t channel);

    void Init();

    void Start(uint8_t percent);
    void StartTimed(uint8_t percent, uint32_t ms);
    void pass();

     void resume();

    void Stop();

    bool IsRunning();   // auto handles timeout

private:
    uint32_t map(uint32_t val,
                 uint32_t in_min, uint32_t in_max,
                 uint32_t out_min, uint32_t out_max);

    TIM_HandleTypeDef *htim;
    uint32_t channel;

    uint32_t startTime,remaning;
    uint32_t runTime;
    uint32_t pwm;
    bool running;
    bool timedMode;
    bool _pass;
};


#endif /* INC_VACUUMPUMP_H_ */
