/*
 * SysMain.h
 *
 *  Created on: 20-Mar-2026
 *      Author: tamil
 */

#ifndef INC_SYSMAIN_H_
#define INC_SYSMAIN_H_

#include <stdint.h>
#include "tim.h"
#include "usart.h"
class SysMain
{
public:
	uint32_t DwinRtcUpdate=0;
	float hold ;
	float _hold ;
	uint8_t buzzerState;
	uint8_t buzzer;
    /* ================== ENUMS ================== */
    enum  ModeData : uint8_t
    {
        GYNO = 0,
        FINNEEDEL,
        MCUSPEC,
        BODYFULID,
        ECTUSER,
        BRUSHSCRAPING
    };

    enum  UiState : uint8_t
    {
        UI_IDEAL = 0,
        PRS_START,
        PRS_STOP,
		PRS_STOP_RUN,
        PRS_RESET,
        UPDATE_MODES,
        UPDATE_RTC,
        UI_PROCESS_DONE,
        SEQ_CHANGE,
        UI_ERROR
    };

    enum  SysSleep : uint8_t
    {
        HOME_TIGER = 0,
        SLEEP_MODE,
        LOADDE_ICON
    };



    struct __attribute__((packed)) SysProcess
      {
    	UiState state;
    	ModeData mode;
    	ModeData setmode;
      };

    SysProcess sysProcess;



    /* ================== STRUCTS ================== */
    struct __attribute__((packed)) ModeConfig
    {
        int status;
        int vctime;
        int vcper;
        int transhold;
        int transpower;

    };

    struct __attribute__((packed)) AllModesData
    {
        ModeConfig modes[6];
    };

    AllModesData CurrentDataUpdate;
    AllModesData processData;
    static AllModesData DefaultModeData;

    /* ================== VARIABLES ================== */
    static uint8_t RxBuff;

    /* ================== FUNCTIONS ================== */
     void UiLoadingPage(SysSleep sysSleep);
     void SysUpdate(void);
     void SystemInit(void);
     void NewDateLogFile(void);
     void SystemProcess(void);
     void SPLPumpValveControl(void);
     uint32_t calculate_step_pulses_for_mm(float distance_mm);
     uint32_t mapValue(uint32_t value,
                             uint32_t in_min,
                             uint32_t in_max,
                             uint32_t out_min,
                             uint32_t out_max);

     float mapFloat(float value,
                          float in_min,
                          float in_max,
                          float out_min,
                          float out_max);
};

extern SysMain sysMain;
#endif /* INC_SYSMAIN_H_ */
