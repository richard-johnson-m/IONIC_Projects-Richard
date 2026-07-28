/*
 * sysmain.cpp
 *
 *  Created on: 20-Mar-2026
 *      Author: tamil
 */

#include "main.h"
#include "sysmain.h"
#include "eeprom.h"
#include "xRtcHand.h"
#include "DCMotor.h"
#include "Stepper.h"
#include "SystemController.h"
#include "SysMain.h"
#include "DWIN_UI.h"
#include <cstring>
/* ================== STATIC VARIABLE ================== */
SysMain sysMain;
DCMotor motor(&htim3, TIM_CHANNEL_3, &htim1, TIM_CHANNEL_2,
		Slider_EN_GPIO_Port, Slider_EN_Pin, SLIDER_HOME_GPIO_Port, SLIDER_HOME_Pin,
		SLIDER_END_GPIO_Port, SLIDER_END_Pin);

Stepper stepper(&htim2, TIM_CHANNEL_1,
		Linear_DIR_GPIO_Port, Linear_DIR_Pin, Linear_EN_GPIO_Port, Linear_EN_Pin,
		LINEAR_HOME_GPIO_Port, LINEAR_HOME_Pin, LINEAR_END_GPIO_Port, LINEAR_END_Pin);
VacuumPump vacuumpump(&htim3, TIM_CHANNEL_1);
SystemController system(&stepper, &motor, &vacuumpump);
eeprom EEPROM;
static DWIN_UI ui;
/* ================== FUNCTIONS ================== */

uint8_t SysMain::RxBuff = 0;

SysMain::AllModesData SysMain::DefaultModeData = { { { 1,5, 50, 40, 50 }, { 1,6,
		60, 30, 50 }, { 1,6, 70, 40, 50}, { 1,6, 40, 50, 50 }, { 1,6, 30,
				50, 50 }, { 1,6, 50, 50, 50 } } };

void SysMain::SysUpdate(void) {
	// TODO: Implement system update logic
}

void SysMain::SystemInit(void) {

	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
	HAL_UART_Receive_IT(&huart3, (uint8_t*) &RxBuff, 1);
	HAL_Delay(1600);

	ui.LodingPage();

	HAL_Delay(1000);
	stepper.Init(16, 200);
	vacuumpump.Init();
	motor.Init();
	system.Init();
	while (!(system.state == system.SYS_READY)) {
		system.Update();
	}

	ui.wakeSystem();

	AllModesData readData;
	EEPROM.EEPROM_Read(0x0000, (uint8_t*) &readData, sizeof(readData));

	if ((readData.modes[GYNO].status != 1)
			|| (readData.modes[FINNEEDEL].status != 1)
			|| (readData.modes[MCUSPEC].status != 1)
			|| (readData.modes[BODYFULID].status != 1)
			|| (readData.modes[BRUSHSCRAPING].status != 1)
			|| (readData.modes[ECTUSER].status != 1)) {
		EEPROM.EEPROM_Write(0x0000, (uint8_t*) &DefaultModeData,
				sizeof(DefaultModeData));
		memcpy(&CurrentDataUpdate, &DefaultModeData, sizeof(DefaultModeData));

	} else {
		memcpy(&CurrentDataUpdate, &readData, sizeof(readData));
	}

	DwinRtcUpdate = HAL_GetTick();

	//	Ui_loadding_page(HOME_TIGER);
	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
	ui.uartMon = HAL_GetTick();

}

void SysMain::SystemProcess(void) {

	if (ui.header_Rec_Flag == 1) {
		ui.header_Rec_Flag = 0;

		ui.handlePageSwitch((DWIN_UI::VP) ui.rec.vp_Address, ui.rec.touchData);
	}

	system.Update();


	switch (sysProcess.state) {
	case UI_IDEAL:

		break;
	case PRS_START:
		system.vacuum_time =1000*sysMain.CurrentDataUpdate.modes[sysMain.sysProcess.mode].vctime;
		system.vacuum_presure =sysMain.CurrentDataUpdate.modes[sysMain.sysProcess.mode].vcper-1;
		buzzer=1;

		 hold =(float)sysMain.CurrentDataUpdate.modes[sysMain.sysProcess.mode].transpower;
		 _hold= mapFloat(hold,1.0,100.0,1250.0,1300.0);
		system.smire_presure = calculate_step_pulses_for_mm(_hold);

		system.smire_time =1000*sysMain.CurrentDataUpdate.modes[sysMain.sysProcess.mode].transhold;

		system.Start();
		sysProcess.state = UI_PROCESS_DONE;
		break;
	case PRS_STOP:
		buzzer=0;
		system.Pause();
		break;
	case PRS_STOP_RUN:
		buzzer=1;
		system.Resume();
		sysProcess.state = UI_PROCESS_DONE;
		break;
	case PRS_RESET:

		system.Init();
		sysProcess.state = UI_PROCESS_DONE;
		break;
	case UPDATE_MODES:
		EEPROM.EEPROM_Write(0x0000, (uint8_t*) &CurrentDataUpdate,
				sizeof(CurrentDataUpdate));

		if (CurrentDataUpdate.modes[sysMain.sysProcess.mode].status != 1) {
			memcpy(&CurrentDataUpdate, &DefaultModeData,
					sizeof(DefaultModeData));
		}
		sysProcess.state = UI_IDEAL;
		break;
	case UPDATE_RTC:
		if(ui.rtcData.year>=25)
			{
			rtc.Set_Time_n_Date(ui.rtcData.sec,ui.rtcData.min,ui.rtcData.hours,1,ui.rtcData.day,ui.rtcData.month,ui.rtcData.year);

			}
		sysProcess.state = UI_IDEAL;
		break;
	case UI_PROCESS_DONE:
		if ((system.state == system.SYS_DONE)
				|| (system.state == system.SYS_READY)) {
			HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
			HAL_Delay(500);
			HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
			sysProcess.state = UI_IDEAL;
			buzzer=0;
			ui.homeTriger();
			HAL_GPIO_WritePin(WATER_OUT_GPIO_Port, WATER_OUT_Pin, GPIO_PIN_RESET);
		}
		break;

	}




	static uint32_t previousMillis=0;
	if (buzzer == 1)
	{
//		stopbuzzer=1;
		uint32_t _currentMillis = HAL_GetTick();

		if (buzzerState == 0 && (_currentMillis - previousMillis >= 2000))
		{
			HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
			buzzerState = 1;
			previousMillis = _currentMillis;
		}
		else if (buzzerState == 1 && (_currentMillis - previousMillis >= 10))
		{
			HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
			buzzerState = 0;
			previousMillis = _currentMillis;
		}
	}
	else
	{
		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
		buzzerState = 0;
		previousMillis = HAL_GetTick();
	}




	static uint32_t pressStartTime = 0;
	static bool timingStarted = false;
//TODO
#warning
	if (!HAL_GPIO_ReadPin(POWER_ON_GPIO_Port, POWER_ON_Pin)) // active LOW
	{
	    if (!timingStarted)
	    {
	        pressStartTime = HAL_GetTick();  // start timer
	        timingStarted = true;
	    }

	    if (HAL_GetTick() - pressStartTime >= 2000)
	    {
	    	ui.SleepMode();
			if (system.state < 16) {
				system.Pause();
				HAL_Delay(100);
				system.Reset();
			}
			if (system.state == system.SYS_POFF) {

				HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
				HAL_Delay(1500);

				HAL_GPIO_WritePin(POWER_ON_OFF_GPIO_Port, POWER_ON_OFF_Pin,
						GPIO_PIN_RESET);
			}
	    }
	}
	else
	{

	    timingStarted = false;
	    pressStartTime=HAL_GetTick();
	}
	if (!HAL_GPIO_ReadPin(POWER_ON_GPIO_Port, POWER_ON_Pin)) {

	}

	if ((HAL_GetTick() - DwinRtcUpdate) >= 1000) {
		rtc.Get_Time_n_Date();
		DwinRtcUpdate = HAL_GetTick();
		ui.sendCurrentDateTimeToDwin();
	}
}

/* ================== UTILITY FUNCTIONS ================== */

uint32_t SysMain::mapValue(uint32_t value, uint32_t in_min, uint32_t in_max,
		uint32_t out_min, uint32_t out_max) {
	if (in_max == in_min)
		return out_min; // avoid div0
	if (value <= in_min)
		return out_min;
	if (value >= in_max)
		return out_max;
	return (uint32_t) ((uint64_t) (value - in_min) * (out_max - out_min)
			/ (in_max - in_min) + out_min);
}

float SysMain::mapFloat(float value, float in_min, float in_max, float out_min,
		float out_max) {
	if (in_max == in_min)
		return out_min; // avoid div0
	if (value <= in_min)
		return out_min;
	if (value >= in_max)
		return out_max;
	return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint32_t SysMain::calculate_step_pulses_for_mm(float distance_mm) {
	distance_mm=distance_mm/100;
	uint32_t steps_per_revolution = 200 *16;
	float steps_needed = (float)((distance_mm / 8) * (float)steps_per_revolution);
	return (uint32_t)steps_needed;
}

extern "C" void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {

	stepper.HandlePulse();

}

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	stepper.HandleExti(GPIO_Pin);

	motor.HandleExti(GPIO_Pin);

}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == USART3) {
		ui.get_HMI_Input_ISR(SysMain::RxBuff);
		HAL_UART_Receive_IT(&huart3, (uint8_t*) &SysMain::RxBuff, 1);
		ui.uartMon = HAL_GetTick();
	}

}
