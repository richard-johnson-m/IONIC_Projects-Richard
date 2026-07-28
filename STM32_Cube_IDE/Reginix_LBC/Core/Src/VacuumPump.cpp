/*
 * VacuumPump.cpp
 *
 *  Created on: 22-Mar-2026
 *      Author: tamil
 */


#include "VacuumPump.h"
#include "main.h"
VacuumPump::VacuumPump(TIM_HandleTypeDef *htim, uint32_t channel)
{
	this->htim = htim;
	this->channel = channel;

	running = false;
	timedMode = false;
}

void VacuumPump::Init()
{
	Stop();
	HAL_GPIO_WritePin(WATER_OUT_GPIO_Port, WATER_OUT_Pin, GPIO_PIN_RESET);
}

uint32_t VacuumPump::map(uint32_t val,
		uint32_t in_min, uint32_t in_max,
		uint32_t out_min, uint32_t out_max)
{
	if (val <= in_min) return out_min;
	if (val >= in_max) return out_max;

	return (val - in_min) * (out_max - out_min) /
			(in_max - in_min) + out_min;
}

void VacuumPump::Start(uint8_t percent)
{
	HAL_GPIO_WritePin(WATER_OUT_GPIO_Port, WATER_OUT_Pin, GPIO_PIN_RESET);
	if(percent<1)
	{
		percent=1;
	}
	else if(percent>100)
	{
		percent=99;
	}
	else
	{
		pwm = map(percent, 1, 100, 600, 1200);
	}


	__HAL_TIM_SET_COMPARE(htim, channel, pwm);
	HAL_TIM_PWM_Start(htim, channel);

	running = true;
	//    timedMode = false;
}

void VacuumPump::StartTimed(uint8_t percent, uint32_t ms)
{
	Start(percent);

	startTime = HAL_GetTick();
	runTime = ms;

	timedMode = true;
}

void VacuumPump::Stop()
{

	HAL_TIM_PWM_Stop(htim, channel);
	__HAL_TIM_SET_COMPARE(htim, channel, 0);
	HAL_Delay(5);
	HAL_GPIO_WritePin(WATER_OUT_GPIO_Port, WATER_OUT_Pin, GPIO_PIN_SET);
	startTime=0;
	remaning=0;
	running = false;
	timedMode = false;
	_pass=false;
}
void VacuumPump::pass()
{

	_pass=true;
	if (running && timedMode)
	{

		uint32_t timechk = (HAL_GetTick()-startTime);

        if (timechk >= runTime)
        {
            remaning = 0;
        }
        else
        {
            remaning = runTime - timechk;
        }
		HAL_TIM_PWM_Stop(htim, channel);
		__HAL_TIM_SET_COMPARE(htim, channel, 0);
	}
}
void VacuumPump::resume()
{
	_pass=false;
	if (running && timedMode)
	{
		startTime = HAL_GetTick() - (runTime - remaning);
		__HAL_TIM_SET_COMPARE(htim, channel, pwm);
		HAL_TIM_PWM_Start(htim, channel);
	}
}
bool VacuumPump::IsRunning()
{
	// Auto stop check inside
	if (running && timedMode)
	{
		if(!_pass)
		{
			if (HAL_GetTick() - startTime >= runTime)
			{
				Stop();
			}
		}

	}

	return running;
}

