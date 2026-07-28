/*
 * DWIN_UI.h
 *
 *  Created on: 20-Mar-2026
 *      Author: tamil
 */

#ifndef INC_DWIN_UI_H_
#define INC_DWIN_UI_H_


#include "stm32f1xx.h"
#include "tim.h"
#include "main.h"
#include "usart.h"
#include "xRtcHand.h"
/* ================== CLASS ================== */
class DWIN_UI
{
public:
    DWIN_UI();

    char passwordBuffer[4 + 1] = {0};
    uint8_t passwordOK;
	uint8_t passIndex = 0;
    /* ================== ENUMS ================== */
    enum  VP : uint16_t
    {
    	GYNO_IDLE_VP = 0x1000,
    		GYNO_PROCESS_PAUSE_VP = 0x1002,
    		GYNO_START_VP = 0x1004,
    		GYNO_SELECTION_VP = 0X1008,
    		GYNO_PARA_VP = 0x100A,


    		FINENEEDLE_IDLE_VP = 0x100C,
    		FINENEEDLE_PROCESS_PAUSE_VP= 0x1010,
    		FINENEEDLE_START_VP = 0x1012,
    		FINENEEDLE_SELECTION_VP = 0x100E,
    		FINENEEDLE_PARA_VP = 0x104E,


    		MUCOID_IDLE_VP = 0x1014,
    		MUCOID_PROCESS_PAUSE_VP= 0x1016,
    		MUCOID_START_VP = 0x1018,
    		MUCOID_SELECTION_VP = 0x101A,
    		MUCOID_PARA_VP = 0x101C,



    		BODYFLUID_IDLE_VP = 0x101E,
    		BODYFLUID_PROCESS_PAUSE_VP = 0x1020,
    		BODYFLUID_START_VP = 0x1022,
    		BODYFLUID_SELECTION_VP = 0x1024,
    		BODYFLUID_PARA_VP = 0x1026,



    		ECT_IDLE_VP = 0x1028,
    		ECT_PROCESS_PAUSE_VP = 0x102A,
    		ECT_START_VP = 0x102C,
    		ECT_SELECTION_VP = 0x102E,
    		ECT_PARA_VP = 0x1030,

    		SUPERFICIAL_IDLE_VP = 0x1050,
    		SUPERFICIAL_PROCESS_PAUSE_VP = 0x1032,
    		SUPERFICIAL_START_VP = 0x1034,
    		SUPERFICIAL_SELECTION_VP = 0x1036,
    		SUPERFICIAL_PARA_VP = 0x1038,


    		MODE_VP = 0x1006,
    		PSW_VP = 0x103C,
    		///////////////////////////////////////////////
    		eGYNO_VCTIME_VP = 0x2052,
    		eGYNO_VCPER_VP = 0x2054,
    		eGYNO_TRANSHOLD_VP = 0x2056,
    		eGYNO_TRANSPOWER_VP = 0x2058,


    		eFINENEEDLE_VCTIME_VP = 0x205A,
    		eFINENEEDLE_VCPER_VP = 0x205C,
    		eFINENEEDLE_TRANSHOLD_VP = 0x205E,
    		eFINENEEDLE_TRANSPOWER_VP = 0x2060,



    		eMUCOID_VCTIME_VP = 0x2062,
    		eMUCOID_VCPER_VP = 0x2064,
    		eMUCOID_TRANSHOLD_VP = 0x2066,
    		eMUCOID_TRANSPOWER_VP = 0x2068,


    		eBODYFLUID_VCTIME_VP = 0x2070,
    		eBODYFLUID_VCPER_VP = 0x2072,
    		eBODYFLUID_TRANSHOLD_VP = 0x2074,
    		eBODYFLUID_TRANSPOWER_VP = 0x2076,

    		eECT_VCTIME_VP = 0x2078,
    		eECT_VCPER_VP = 0x207A,
    		eECT_TRANSHOLD_VP = 0x207C,
    		eECT_TRANSPOWER_VP = 0x207E,


    		eSUPERFICIAL_VCTIME_VP = 0x2080,
    		eSUPERFICIAL_VCPER_VP = 0x2090,
    		eSUPERFICIAL_TRANSHOLD_VP = 0x2082,
    		eSUPERFICIAL_TRANSPOWER_VP = 0x2084,
    	/////////////////////////////////////////////
    		GYNO_VCTIME_VP = 0x1052,
    		GYNO_VCPER_VP = 0x1054,
    		GYNO_TRANSHOLD_VP = 0x1056,
    		GYNO_TRANSPOWER_VP = 0x1058,

    		FINENEEDLE_VCTIME_VP = 0x105A,
    		FINENEEDLE_VCPER_VP = 0x105C,
    		FINENEEDLE_TRANSHOLD_VP = 0x105E,
    		FINENEEDLE_TRANSPOWER_VP = 0x1060,

    		MUCOID_VCTIME_VP = 0x1062,
    		MUCOID_VCPER_VP = 0x1064,
    		MUCOID_TRANSHOLD_VP = 0x1066,
    		MUCOID_TRANSPOWER_VP = 0x1068,

    		BODYFLUID_VCTIME_VP = 0x1070,
    		BODYFLUID_VCPER_VP = 0x1072,
    		BODYFLUID_TRANSHOLD_VP = 0x1074,
    		BODYFLUID_TRANSPOWER_VP = 0x1076,

    		ECT_VCTIME_VP = 0x1078,
    		ECT_VCPER_VP = 0x107A,
    		ECT_TRANSHOLD_VP = 0x107C,
    		ECT_TRANSPOWER_VP = 0x107E,

    		SUPERFICIAL_VCTIME_VP = 0x1080,
    		SUPERFICIAL_VCPER_VP = 0x1090,
    		SUPERFICIAL_TRANSHOLD_VP = 0x1082,
    		SUPERFICIAL_TRANSPOWER_VP = 0x1084,
    	////////////////////////////////////////////////////////
    		RTC_ENTER_VP = 0x1048,
    		RTC_HOME_VP = 0x104C,

    		RTC_TEXT_BOX_DATE_VP = 0x5000,
    		RTC_TEXT_BOX_TIME_VP = 0x5100,

    		PSW_TEXT_BOX_VP = 0x2002,
    		LOADING_PAGE_VP = 0x3000,

    		RTC_DAY_VP = 0X2004,
    		RTC_MONTH_VP = 0X2006,
    		RTC_YEAR_VP = 0X2008,


    		RTC_HOURS_VP = 0X200A,
    		RTC_MIN_VP = 0X200C,
    		RTC_SEC_VP = 0X200E,

    		CONTROL_HUB_VP =0X2010,


    		WAKETOUCH_VP = 0X2012,

    		PSW_TEXT_VP = 0X4000,

    };

    enum  Page : uint8_t
    {
        LOADING_PAGE = 43,

        GYNO_IDLE_PAGE = 0,
        GYNO_PROCESS_PAUSE_PAGE = 1,
        GYNO_START_PAGE = 2,
        MODE_PAGE = 4,
        GYNO_SELECTION_PAGE = 5,
        GYNO_PARA_PAGE = 6,

        FINENEEDLE_IDLE_PAGE = 7,
        FINENEEDLE_SELECTION_PAGE = 8,
        FINENEEDLE_PARA_PAGE = 9,
        FINENEEDLE_PROCESS_PAUSE_PAGE = 10,
        FINENEEDLE_START_PAGE = 11,

        MUCOID_IDLE_PAGE = 13,
        MUCOID_PROCESS_PAUSE_PAGE = 14,
        MUCOID_START_PAGE = 15,
        MUCOID_SELECTION_PAGE = 17,
        MUCOID_PARA_PAGE = 18,

        BODYFLUID_IDLE_PAGE = 19,
        BODYFLUID_PROCESS_PAUSE_PAGE = 20,
        BODYFLUID_START_PAGE = 21,
        BODYFLUID_SELECTION_PAGE = 23,
        BODYFLUID_PARA_PAGE = 24,

        ECT_IDLE_PAGE = 25,
        ECT_PROCESS_PAUSE_PAGE = 26,
        ECT_START_PAGE = 27,
        ECT_SELECTION_PAGE = 29,
        ECT_PARA_PAGE = 30,

        SUPERFICIAL_IDLE_PAGE = 31,
        SUPERFICIAL_START_PAGE = 33,
        SUPERFICIAL_PROCESS_PAUSE_PAGE = 34,
        SUPERFICIAL_SELECTION_PAGE = 35,
        SUPERFICIAL_PARA_PAGE = 36,

        CONTROL_HUB_PAGE = 37,
        PASSWORD_PAGE = 38,
        RTC_SETTING_PAGE = 42
    };

    /* ================== STRUCTS ================== */

#pragma pack(push, 1)
    typedef union {
        struct {
        	uint8_t headerHigh;
        	uint8_t headerLow;
        	uint8_t frameData_Lenght;
        	uint8_t touch_CMD;
        	uint16_t vp_Address;
        	uint8_t wordLength;
        	uint16_t  touchData;
        	uint16_t  touchData1;
        	uint16_t  touchData2;
        	uint16_t  touchData3;
        };

        uint8_t rawData[20];  // Raw data received from UART
    } sDWIN_Rx;
#pragma pack(pop)

    struct OprPara
    {
    	uint8_t mode =0;
        uint8_t vctime = 0;
        uint8_t vcpress = 0;
        uint8_t transhold = 0;
        uint8_t transpower = 0;
    };

    struct _rtc
    {
        uint8_t month = 0, year = 0, day = 0;
        uint8_t hours = 0, min = 0, sec = 0;
    };

    struct Flags
    {
        bool gyno_flag = false;
        bool fine_flag = false;
        bool mucoid_flag = false;
        bool fluid_flag = false;
        bool ect_flag = false;
        bool superficial_flag = false;
        bool controlhub = false;
        bool LOG = false;
    };

    struct Run
    {
        uint8_t start = 0;
        uint8_t stop = 0;
        uint8_t reset = 0;
    };

    /* ================== VARIABLES ================== */
    uint32_t uartMon = 0;
    uint32_t passWord = 0;
    uint32_t default_passWord = 1234;

    sDWIN_Rx rec{};
    OprPara mode_para{};
    OprPara Getmode_para{};
    OprPara resetPara{};

    _rtc rtcData{};


    Flags modeFlags{};
    Flags pswFlags{};

    uint8_t rxByte = 0;
    uint8_t headerFlag = 0;

    int sys_finished = 0;
    uint8_t erroruart;
    uint8_t header_Rec_Flag;


    uint8_t prev_transpower;
	uint8_t prev_transhold;
	uint8_t prev_vcpress;
	uint8_t prev_vctime;
	int16_t temp;
    /* ================== CORE FUNCTIONS ================== */
    void handlePageSwitch(VP vp_Address, uint16_t touchData);

    //void get_HMI_Input_ISR(uint8_t Rx_Buff) ;
    void get_HMI_Input_ISR(uint8_t b) ;
    // Declare the handlePageSwitch function and all the process functions
    void gyno_Home_Page(uint16_t touchData);
    void gyno_process_on_Going_Page(uint16_t touchData);
    void gyno_stop_Page(uint16_t touchData);
    void Mode_Page(uint16_t touchData);
    void gyno_selection_Page(uint16_t touchData);
    void gyno_settings_Page(uint16_t touchData);
    void Fine_Needle_Home_Page(uint16_t touchData);
    void Fine_Needle_process_on_Going_Page(uint16_t touchData);
    void Fine_Needle_stop_Page(uint16_t touchData);
    void Fine_Needle_selection_Page(uint16_t touchData);
    void Fine_Needle_settings_Page(uint16_t touchData);
    void Mucoid_Home_Page(uint16_t touchData);
    void Mucoid_process_on_Going_Page(uint16_t touchData);
    void Mucoid_stop_Page(uint16_t touchData);
    void Mucoid_selection_Page(uint16_t touchData);
    void Mucoid_settings_Page(uint16_t touchData);
    void fluid_Home_Page(uint16_t touchData);
    void fluid_process_on_Going_Page(uint16_t touchData);
    void fluid_stop_Page(uint16_t touchData);
    void fluid_selection_Page(uint16_t touchData);
    void fluid_settings_Page(uint16_t touchData);
    void ECT_Home_Page(uint16_t touchData);
    void ECT_process_on_Going_Page(uint16_t touchData);
    void ECT_stop_Page(uint16_t touchData);
    void ECT_selection_Page(uint16_t touchData);
    void ECT_settings_Page(uint16_t touchData);
    void superficial_Home_Page(uint16_t touchData);
    void superficial_process_on_Going_Page(uint16_t touchData);
    void superficial_stop_Page(uint16_t touchData);
    void superficial_selection_Page(uint16_t touchData);
    void superficial_settings_Page(uint16_t touchData);
    void restartHMI();
    void Psw_(uint16_t touchData);
    void Psw_enter_button();
    void Psw_back_button();
    void pageSwitch(uint8_t _pageIndex);
    void sendValueToDwin(uint16_t _vpAddress, uint16_t _vpValue);
    void sendFloatValueToDwin(uint16_t _vpAddress, float _vpValue);
    void sendStringToDwin(uint16_t _vpAddress, const char* str);
    void RTC_Enter(uint16_t touchData);
    //void datalog(uint16_t touchData);
    //void LogKeypad(uint16_t touchData);
    //void RTC_back(uint16_t touchData);
    void sendCurrentDateTimeToDwin(void);
    void LodingPage(void);
    void SleepMode(void);
    void wakeSystem(void);
    void controllhub(uint16_t touchData);
    void homeTriger(void);

    uint8_t safeUpdate(uint8_t newVal, uint8_t prevVal);
};

#endif /* INC_DWIN_UI_H_ */
