/*
 * Stepper.h
 *
 *  Created on: 20-Mar-2026
 *      Author: tamil
 */
#ifndef INC_STEPPER_H_
#define INC_STEPPER_H_

#include "stm32f1xx_hal.h"

class Stepper
{
public:
    Stepper(TIM_HandleTypeDef *htim, uint32_t channel,
            GPIO_TypeDef *dirPort, uint16_t dirPin,
            GPIO_TypeDef *enPort, uint16_t enPin,
            GPIO_TypeDef *homePort, uint16_t homePin,
            GPIO_TypeDef *endPort, uint16_t endPin);

    void Init(uint16_t microstep, uint32_t stepsPerRev);

    void Enable();
    void Disable();

    void SetSpeed(float rpm);

    void MoveForward(uint32_t pulses, uint32_t speed);
    void MoveReverse(uint32_t pulses, uint32_t speed);

    void MoveHome(uint32_t speed);
    void MoveEnd(uint32_t speed);

    void Stop();
    void pass();
    void resume();

    bool IsBusy();
    bool IsDone();

    // ISR handlers
    void HandlePulse();
    void HandleExti(uint16_t GPIO_Pin);
    void HandleExtio();

private:
    TIM_HandleTypeDef *htim;
    uint32_t channel;

    GPIO_TypeDef *dirPort;
    uint16_t dirPin;

    GPIO_TypeDef *enPort;
    uint16_t enPin;

    GPIO_TypeDef *homePort;
    uint16_t homePin;

    GPIO_TypeDef *endPort;
    uint16_t endPin;

    volatile uint32_t targetPulses;
    volatile uint32_t currentPulses;

    volatile bool running;
    volatile bool done;

    uint16_t microstep;
    uint32_t stepsPerRev;

    uint32_t speed;

    enum Mode
    {
        IDLE,
        MOVE_PULSE,
        MOVE_HOME,
        MOVE_END
    } mode;
};

#endif
