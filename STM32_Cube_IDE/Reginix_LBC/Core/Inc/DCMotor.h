/*
 * DCMotor.h
 *
 *  Created on: 20-Mar-2026
 *      Author: tamil
 */

#ifndef INC_DCMOTOR_H_
#define INC_DCMOTOR_H_


#include "stm32f1xx_hal.h" // change if needed

class DCMotor
{
public:
    DCMotor(
        TIM_HandleTypeDef *htim_fwd, uint32_t ch_fwd,
        TIM_HandleTypeDef *htim_rev, uint32_t ch_rev,
        GPIO_TypeDef *enPort, uint16_t enPin,
        GPIO_TypeDef *homePort, uint16_t homePin,
        GPIO_TypeDef *endPort, uint16_t endPin);

    void Init();

    void Enable();
    void Disable();

    void MoveForward(uint16_t speed); // 0–100 %
    void MoveReverse(uint16_t speed);

    void MoveHome(uint16_t speed);
    void MoveEnd(uint16_t speed);

    void Stop();
    void pass();
    void resume();

    bool IsRunning();

    // ISR handlers
    void HandleExti(uint16_t GPIO_Pin);
    void HandleExtio();

private:
    void SetSpeedForward(uint16_t speed);
    void SetSpeedReverse(uint16_t speed);

    TIM_HandleTypeDef *htim_fwd;
    uint32_t ch_fwd;

    TIM_HandleTypeDef *htim_rev;
    uint32_t ch_rev;

    GPIO_TypeDef *enPort;
    uint16_t enPin;

    GPIO_TypeDef *homePort;
    uint16_t homePin;

    GPIO_TypeDef *endPort;
    uint16_t endPin;

    volatile bool running;

    enum Mode
    {
        IDLE,
        FORWARD,
        REVERSE,
        MOVE_HOME,
        MOVE_END
    } mode;
};

// Global pointer for ISR




#endif /* INC_DCMOTOR_H_ */
