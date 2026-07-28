/*
 * DCMotor.cpp
 *
 *  Created on: 20-Mar-2026
 *      Author: tamil
 */


#include "DCMotor.h"

DCMotor::DCMotor(
    TIM_HandleTypeDef *htim_fwd, uint32_t ch_fwd,
    TIM_HandleTypeDef *htim_rev, uint32_t ch_rev,
    GPIO_TypeDef *enPort, uint16_t enPin,
    GPIO_TypeDef *homePort, uint16_t homePin,
    GPIO_TypeDef *endPort, uint16_t endPin)
{
    this->htim_fwd = htim_fwd;
    this->ch_fwd = ch_fwd;

    this->htim_rev = htim_rev;
    this->ch_rev = ch_rev;

    this->enPort = enPort;
    this->enPin = enPin;

    this->homePort = homePort;
    this->homePin = homePin;

    this->endPort = endPort;
    this->endPin = endPin;

    running = false;
    mode = IDLE;


}

void DCMotor::Init()
{
    Stop();
}

void DCMotor::Enable()
{
    HAL_GPIO_WritePin(enPort, enPin, GPIO_PIN_SET); // depends on driver
}

void DCMotor::Disable()
{
    HAL_GPIO_WritePin(enPort, enPin, GPIO_PIN_RESET);
}

void DCMotor::SetSpeedForward(uint16_t speed)
{

    __HAL_TIM_SET_COMPARE(htim_fwd, ch_fwd, speed);
}

void DCMotor::SetSpeedReverse(uint16_t speed)
{

    __HAL_TIM_SET_COMPARE(htim_rev, ch_rev, speed);
}

void DCMotor::MoveForward(uint16_t speed)
{
    HAL_TIM_PWM_Stop(htim_rev, ch_rev);

    SetSpeedForward(speed);

    Enable();
    HAL_TIM_PWM_Start(htim_fwd, ch_fwd);

    running = true;
    mode = FORWARD;
}

void DCMotor::MoveReverse(uint16_t speed)
{
    HAL_TIM_PWM_Stop(htim_fwd, ch_fwd);

    SetSpeedReverse(speed);

    Enable();
    HAL_TIM_PWM_Start(htim_rev, ch_rev);

    running = true;
    mode = REVERSE;
}

void DCMotor::MoveHome(uint16_t speed)
{
    MoveReverse(speed);
    mode = MOVE_HOME;
}

void DCMotor::MoveEnd(uint16_t speed)
{
    MoveForward(speed);
    mode = MOVE_END;
}

void DCMotor::Stop()
{
    HAL_TIM_PWM_Stop(htim_fwd, ch_fwd);
    HAL_TIM_PWM_Stop(htim_rev, ch_rev);
    Disable();
    running = false;
    mode = IDLE;
}

void DCMotor::pass()
{

    Disable();
}
 void DCMotor::resume()
 {
	 Enable();
 }

bool DCMotor::IsRunning()
{
    return running;
}

void DCMotor::HandleExti(uint16_t GPIO_Pin)
{
    // HOME switch hit
    if (GPIO_Pin == homePin && mode == MOVE_HOME)
    {
        Stop();
    }

    // END switch hit
    if (GPIO_Pin == endPin && mode == MOVE_END)
    {
        Stop();
    }
}


void DCMotor::HandleExtio()
{


    // HOME switch hit
    if (HAL_GPIO_ReadPin(homePort, homePin) && mode == MOVE_HOME)
    {
        Stop();
    }

    // END switch hit
    if (HAL_GPIO_ReadPin(endPort, endPin) && mode == MOVE_END)
    {
        Stop();
    }

}
//uint32_t DCMotor::mapValue(uint32_t value,
//		uint32_t in_min,
//		uint32_t in_max,
//		uint32_t out_min,
//		uint32_t out_max)
//{
//	if (in_max == in_min) return out_min; // avoid div0
//	if (value <= in_min) return out_min;
//	if (value >= in_max) return out_max;
//	return (uint32_t)((uint64_t)(value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
//}
//
//float DCMotor::mapFloat(float value,
//		float in_min,
//		float in_max,
//		float out_min,
//		float out_max)
//{
//	if (in_max == in_min) return out_min; // avoid div0
//	if (value <= in_min) return out_min;
//	if (value >= in_max) return out_max;
//	return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
//}



//
// 🔥 EXTI CALLBACK IMPLEMENTED HERE
//

//extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//    if (g_motor != nullptr)
//    {
//        g_motor->HandleExti(GPIO_Pin);
//    }
//}


