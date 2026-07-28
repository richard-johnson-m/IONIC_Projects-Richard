/*
 * DWIN_UI.cpp
 *
 *  Created on: 20-Mar-2026
 *      Author: tamil
 */

#include "DWIN_UI.h"
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include "SystemController.h"
#include "sysmain.h"
#include "xRtcHand.h"

// ================= ISR =================
DWIN_UI::DWIN_UI() {
}
__attribute__((section(".RamFunc")))
void DWIN_UI::get_HMI_Input_ISR(uint8_t b) {
	static uint8_t st = 0, len = 0, idx = 0, wlen = 0;
	static uint8_t buf[32];  // Increased with margin
	const uint8_t MAX_PACKET_LEN = 50;  // Define reasonable maximum

	switch (st) {
	case 0: // Wait for high header
		if (b == 0x5A) {
			idx = 0;
			buf[idx++] = b;
			st = 1;
			wlen = 0;
			len = 0;
		} else {
			erroruart++;
		}
		break;

	case 1: // Expect low header
		if (b == 0XA5) {
			buf[idx++] = b;
			st = 2;
		} else {
			// Reset if sequence broken
			st = 0;
			idx = 0;
			erroruart++;
		}
		break;

	case 2: // Length byte
		// Validate length: account for headers (2) + length (1) + minimal payload
		if (b <= 2 || b > MAX_PACKET_LEN) {
			// Invalid length, reset
			st = 0;
			idx = 0;
			break;
		}
		len = b;
		buf[idx++] = b;

		// Check we have enough buffer space
		if ((2 + len) > sizeof(buf)) {
			st = 0;
			idx = 0;
			erroruart++;
			break;
		}
		st = 3;
		break;

	case 3:
		//touch cmd
		if (b == 0x82 || b == 0x83) {
			buf[idx++] = b;
		} else {
			st = 0;
			idx = 0;
			erroruart++;
			break;
		}
		st = 4;
		break;
	case 4:
		buf[idx++] = b;
		st = 5;
		break;
	case 5:
		buf[idx++] = b;
		if (len == 3) {
			st = 0;
			idx = 0;
			break;
		}
		st = 6;
		break;
	case 6:
		//		datalenth
		wlen = b + 1;
		buf[idx++] = b;
		st = 7;
		break;

	case 7:
		if (wlen == 0) {
			st = 0;
			idx = 0;
			break;
		}
		buf[idx++] = b;
		wlen--;
		if (wlen == 0) {
			uint8_t t = buf[4];
			buf[4] = buf[5];
			buf[5] = t;
			st = 0;
			idx = 0;
			rec.frameData_Lenght = 0;
			rec.headerHigh = 0;
			rec.headerLow = 0;
			rec.touchData = 0;
			rec.touchData1 = 0;
			rec.touchData2 = 0;
			rec.touchData3 = 0;
			rec.touch_CMD = 0;
			rec.vp_Address = 0;
			rec.wordLength = 0;
			rec.rawData[0] = buf[0];
			rec.rawData[1] = buf[1];
			rec.rawData[2] = buf[2];
			rec.rawData[3] = buf[3];
			rec.rawData[4] = buf[4];
			rec.rawData[5] = buf[5];
			rec.rawData[6] = buf[6];
			rec.rawData[7] = buf[8];
			rec.rawData[8] = buf[7];
			rec.rawData[9] = buf[9];
			rec.rawData[10] = buf[10];
			rec.rawData[11] = buf[11];
			rec.rawData[12] = buf[12];
			rec.rawData[13] = buf[13];
			rec.rawData[14] = buf[14];
			rec.rawData[15] = buf[15];
			rec.rawData[16] = buf[16];
			rec.rawData[17] = buf[17];
			rec.rawData[18] = buf[18];
			rec.rawData[19] = buf[19];
			buf[0] = 0;
			buf[1] = 0;
			buf[2] = 0;
			buf[3] = 0;
			buf[4] = 0;
			buf[5] = 0;
			buf[6] = 0;
			buf[7] = 0;
			buf[8] = 0;
			buf[9] = 0;
			buf[10] = 0;
			buf[11] = 0;
			buf[12] = 0;
			buf[13] = 0;
			buf[14] = 0;
			buf[15] = 0;
			buf[16] = 0;
			buf[17] = 0;
			buf[18] = 0;
			buf[19] = 0;
			header_Rec_Flag = 1;
			//			SysProcess.Sleep.GetActiveTicks = Get1000msTiks();
		}

		break;

	default:
		st = 0;
		idx = 0;
		len = 0;
		erroruart++;
		break;
	}
}

void DWIN_UI::handlePageSwitch(VP vp_Address, uint16_t touchData) {
	switch (vp_Address) {
	case GYNO_IDLE_VP:
		gyno_Home_Page(touchData);
		break;
	case GYNO_PROCESS_PAUSE_VP:
		gyno_process_on_Going_Page(touchData);
		break;
	case GYNO_START_VP:
		gyno_stop_Page(touchData);
		break;
	case MODE_VP:
		Mode_Page(touchData);
		break;
	case GYNO_SELECTION_VP:
		gyno_selection_Page(touchData);
		break;
	case GYNO_PARA_VP:
		gyno_settings_Page(touchData);
		break;
	case FINENEEDLE_IDLE_VP:
		Fine_Needle_Home_Page(touchData);
		break;
	case FINENEEDLE_PROCESS_PAUSE_VP:
		Fine_Needle_process_on_Going_Page(touchData);
		break;
	case FINENEEDLE_START_VP:
		Fine_Needle_stop_Page(touchData);
		break;
	case FINENEEDLE_SELECTION_VP:
		Fine_Needle_selection_Page(touchData);
		break;
	case FINENEEDLE_PARA_VP:
		Fine_Needle_settings_Page(touchData);
		break;
	case MUCOID_IDLE_VP:
		Mucoid_Home_Page(touchData);
		break;
	case MUCOID_PROCESS_PAUSE_VP:
		Mucoid_process_on_Going_Page(touchData);
		break;
	case MUCOID_START_VP:
		Mucoid_stop_Page(touchData);
		break;
	case MUCOID_SELECTION_VP:
		Mucoid_selection_Page(touchData);
		break;		//change vpp
	case MUCOID_PARA_VP:
		Mucoid_settings_Page(touchData);
		break;		//change vpp
	case BODYFLUID_IDLE_VP:
		fluid_Home_Page(touchData);
		break;
	case BODYFLUID_PROCESS_PAUSE_VP:
		fluid_process_on_Going_Page(touchData);
		break;
	case BODYFLUID_START_VP:
		fluid_stop_Page(touchData);
		break;
	case BODYFLUID_SELECTION_VP:
		fluid_selection_Page(touchData);
		break;
	case BODYFLUID_PARA_VP:
		fluid_settings_Page(touchData);
		break;
	case ECT_IDLE_VP:
		ECT_Home_Page(touchData);
		break;
	case ECT_PROCESS_PAUSE_VP:
		ECT_process_on_Going_Page(touchData);
		break;
	case ECT_START_VP:
		ECT_stop_Page(touchData);
		break;
	case ECT_SELECTION_VP:
		ECT_selection_Page(touchData);
		break;
	case ECT_PARA_VP:
		ECT_settings_Page(touchData);
		break;
	case SUPERFICIAL_IDLE_VP:
		superficial_Home_Page(touchData);
		break;
	case SUPERFICIAL_PROCESS_PAUSE_VP:
		superficial_process_on_Going_Page(touchData);
		break;
	case SUPERFICIAL_START_VP:
		superficial_stop_Page(touchData);
		break;
	case SUPERFICIAL_SELECTION_VP:
		superficial_selection_Page(touchData);
		break;
	case SUPERFICIAL_PARA_VP:
		superficial_settings_Page(touchData);
		break;
	case PSW_VP:
		Psw_(touchData);
		break;

	case GYNO_VCTIME_VP:
		temp = rec.touchData - prev_vctime;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.vctime = rec.touchData;
			prev_vctime = rec.touchData;
		} else {
			Getmode_para.vctime = prev_vctime;
		}
		sendValueToDwin(GYNO_VCTIME_VP, Getmode_para.vctime);
		sendValueToDwin(eGYNO_VCTIME_VP, Getmode_para.vctime);

		break;
	case FINENEEDLE_VCTIME_VP:

		temp = rec.touchData - prev_vctime;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.vctime = rec.touchData;
			prev_vctime = rec.touchData;
		} else {
			Getmode_para.vctime = prev_vctime;
		}
		//		Getmode_para.vctime = rec.touchData;
		sendValueToDwin(FINENEEDLE_VCTIME_VP, Getmode_para.vctime);

		sendValueToDwin(eFINENEEDLE_VCTIME_VP, Getmode_para.vctime);

		break;
	case MUCOID_VCTIME_VP:
		//		Getmode_para.vctime = rec.touchData;
		temp = rec.touchData - prev_vctime;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.vctime = rec.touchData;
			prev_vctime = rec.touchData;
		} else {
			Getmode_para.vctime = prev_vctime;
		}
		sendValueToDwin(MUCOID_VCTIME_VP, Getmode_para.vctime);

		sendValueToDwin(eMUCOID_VCTIME_VP, Getmode_para.vctime);

		break;
	case BODYFLUID_VCTIME_VP:
		//		Getmode_para.vctime = rec.touchData;
		temp = rec.touchData - prev_vctime;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.vctime = rec.touchData;
			prev_vctime = rec.touchData;
		} else {
			Getmode_para.vctime = prev_vctime;
		}
		sendValueToDwin(BODYFLUID_VCTIME_VP, Getmode_para.vctime);

		sendValueToDwin(eBODYFLUID_VCTIME_VP, Getmode_para.vctime);

		break;
	case ECT_VCTIME_VP:
		//		Getmode_para.vctime = rec.touchData;
		temp = rec.touchData - prev_vctime;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.vctime = rec.touchData;
			prev_vctime = rec.touchData;
		} else {
			Getmode_para.vctime = prev_vctime;
		}
		sendValueToDwin(ECT_VCTIME_VP, Getmode_para.vctime);

		sendValueToDwin(eECT_VCTIME_VP, Getmode_para.vctime);

		break;
	case SUPERFICIAL_VCTIME_VP:
		//		Getmode_para.vctime = rec.touchData;
		temp = rec.touchData - prev_vctime;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.vctime = rec.touchData;
			prev_vctime = rec.touchData;
		} else {
			Getmode_para.vctime = prev_vctime;
		}
		sendValueToDwin(SUPERFICIAL_VCTIME_VP, Getmode_para.vctime);

		sendValueToDwin(eSUPERFICIAL_VCTIME_VP, Getmode_para.vctime);

		break;
		/////////////////////////////////////////////////////////////////////////
	case GYNO_VCPER_VP:
		//		Getmode_para.vcpress = rec.touchData;
		temp = rec.touchData - prev_vcpress;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.vcpress = rec.touchData;
			prev_vcpress = rec.touchData;
		} else {
			Getmode_para.vcpress = prev_vcpress;
		}
		sendValueToDwin(GYNO_VCPER_VP, Getmode_para.vcpress);
		sendValueToDwin(eGYNO_VCPER_VP, Getmode_para.vcpress);
		break;
	case FINENEEDLE_VCPER_VP:
		temp = rec.touchData - prev_vcpress;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.vcpress = rec.touchData;
			prev_vcpress = rec.touchData;
		} else {
			Getmode_para.vcpress = prev_vcpress;
		}
		sendValueToDwin(FINENEEDLE_VCPER_VP, Getmode_para.vcpress);

		sendValueToDwin(eFINENEEDLE_VCPER_VP, Getmode_para.vcpress);

		break;
	case MUCOID_VCPER_VP:
		temp = rec.touchData - prev_vcpress;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.vcpress = rec.touchData;
			prev_vcpress = rec.touchData;
		} else {
			Getmode_para.vcpress = prev_vcpress;
		}
		sendValueToDwin(MUCOID_VCPER_VP, Getmode_para.vcpress);

		sendValueToDwin(eMUCOID_VCPER_VP, Getmode_para.vcpress);

		break;
	case BODYFLUID_VCPER_VP:
		temp = rec.touchData - prev_vcpress;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.vcpress = rec.touchData;
			prev_vcpress = rec.touchData;
		} else {
			Getmode_para.vcpress = prev_vcpress;
		}
		sendValueToDwin(BODYFLUID_VCPER_VP, Getmode_para.vcpress);

		sendValueToDwin(eBODYFLUID_VCPER_VP, Getmode_para.vcpress);

		break;
	case ECT_VCPER_VP:
		temp = rec.touchData - prev_vcpress;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.vcpress = rec.touchData;
			prev_vcpress = rec.touchData;
		} else {
			Getmode_para.vcpress = prev_vcpress;
		}
		sendValueToDwin(ECT_VCPER_VP, Getmode_para.vcpress);

		sendValueToDwin(eECT_VCPER_VP, Getmode_para.vcpress);

		break;
	case SUPERFICIAL_VCPER_VP:
		temp = rec.touchData - prev_vcpress;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.vcpress = rec.touchData;
			prev_vcpress = rec.touchData;
		} else {
			Getmode_para.vcpress = prev_vcpress;
		}
		sendValueToDwin(SUPERFICIAL_VCPER_VP, Getmode_para.vcpress);

		sendValueToDwin(eSUPERFICIAL_VCPER_VP, Getmode_para.vcpress);

		break;
		////////////////////////////////////////////////////////////////////////////////
	case GYNO_TRANSHOLD_VP:
		temp = rec.touchData - prev_transhold;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.transhold = rec.touchData;
			prev_transhold = rec.touchData;
		} else {
			Getmode_para.transhold = prev_transhold;
		}
		sendValueToDwin(GYNO_TRANSHOLD_VP, Getmode_para.transhold);
		sendValueToDwin(eGYNO_TRANSHOLD_VP, Getmode_para.transhold);
		break;
	case FINENEEDLE_TRANSHOLD_VP:
		temp = rec.touchData - prev_transhold;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.transhold = rec.touchData;
			prev_transhold = rec.touchData;
		} else {
			Getmode_para.transhold = prev_transhold;
		}
		sendValueToDwin(FINENEEDLE_TRANSHOLD_VP, Getmode_para.transhold);

		sendValueToDwin(eFINENEEDLE_TRANSHOLD_VP, Getmode_para.transhold);

		break;
	case MUCOID_TRANSHOLD_VP:
		temp = rec.touchData - prev_transhold;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.transhold = rec.touchData;
			prev_transhold = rec.touchData;
		} else {
			Getmode_para.transhold = prev_transhold;
		}
		sendValueToDwin(MUCOID_TRANSHOLD_VP, Getmode_para.transhold);

		sendValueToDwin(eMUCOID_TRANSHOLD_VP, Getmode_para.transhold);

		break;
	case BODYFLUID_TRANSHOLD_VP:
		temp = rec.touchData - prev_transhold;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.transhold = rec.touchData;
			prev_transhold = rec.touchData;
		} else {
			Getmode_para.transhold = prev_transhold;
		}
		sendValueToDwin(BODYFLUID_TRANSHOLD_VP, Getmode_para.transhold);

		sendValueToDwin(eBODYFLUID_TRANSHOLD_VP, Getmode_para.transhold);

		break;
	case ECT_TRANSHOLD_VP:
		temp = rec.touchData - prev_transhold;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.transhold = rec.touchData;
			prev_transhold = rec.touchData;
		} else {
			Getmode_para.transhold = prev_transhold;
		}
		sendValueToDwin(ECT_TRANSHOLD_VP, Getmode_para.transhold);

		sendValueToDwin(eECT_TRANSHOLD_VP, Getmode_para.transhold);

		break;
	case SUPERFICIAL_TRANSHOLD_VP:
		temp = rec.touchData - prev_transhold;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.transhold = rec.touchData;
			prev_transhold = rec.touchData;
		} else {
			Getmode_para.transhold = prev_transhold;
		}
		sendValueToDwin(SUPERFICIAL_TRANSHOLD_VP, Getmode_para.transhold);

		sendValueToDwin(eSUPERFICIAL_TRANSHOLD_VP, Getmode_para.transhold);

		break;

		////////////////////////////////////////////////////////
	case GYNO_TRANSPOWER_VP:
		temp = rec.touchData - prev_transpower;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.transpower = rec.touchData;
			prev_transpower = rec.touchData;
		} else {
			Getmode_para.transpower = prev_transpower;
		}
		sendValueToDwin(GYNO_TRANSPOWER_VP, Getmode_para.transpower);
		sendValueToDwin(eGYNO_TRANSPOWER_VP, Getmode_para.transpower);
		break;
	case FINENEEDLE_TRANSPOWER_VP:
		temp = rec.touchData - prev_transpower;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.transpower = rec.touchData;
			prev_transpower = rec.touchData;
		} else {
			Getmode_para.transpower = prev_transpower;
		}
		sendValueToDwin(FINENEEDLE_TRANSPOWER_VP, Getmode_para.transpower);
		sendValueToDwin(eFINENEEDLE_TRANSPOWER_VP, Getmode_para.transpower);
		break;
	case MUCOID_TRANSPOWER_VP:
		temp = rec.touchData - prev_transpower;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.transpower = rec.touchData;
			prev_transpower = rec.touchData;
		} else {
			Getmode_para.transpower = prev_transpower;
		}
		sendValueToDwin(MUCOID_TRANSPOWER_VP, Getmode_para.transpower);
		sendValueToDwin(eMUCOID_TRANSPOWER_VP, Getmode_para.transpower);
		break;
	case BODYFLUID_TRANSPOWER_VP:
		temp = rec.touchData - prev_transpower;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.transpower = rec.touchData;
			prev_transpower = rec.touchData;
		} else {
			Getmode_para.transpower = prev_transpower;
		}
		sendValueToDwin(BODYFLUID_TRANSPOWER_VP, Getmode_para.transpower);
		sendValueToDwin(eBODYFLUID_TRANSPOWER_VP, Getmode_para.transpower);
		break;
	case ECT_TRANSPOWER_VP:
		temp = rec.touchData - prev_transpower;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.transpower = rec.touchData;
			prev_transpower = rec.touchData;
		} else {
			Getmode_para.transpower = prev_transpower;
		}
		sendValueToDwin(ECT_TRANSPOWER_VP, Getmode_para.transpower);
		sendValueToDwin(eECT_TRANSPOWER_VP, Getmode_para.transpower);
		break;
	case SUPERFICIAL_TRANSPOWER_VP:
		temp = rec.touchData - prev_transpower;
		if ((temp == 1) || (temp == -1)) {
			Getmode_para.transpower = rec.touchData;
			prev_transpower = rec.touchData;
		} else {
			Getmode_para.transpower = prev_transpower;
		}
		sendValueToDwin(SUPERFICIAL_TRANSPOWER_VP, Getmode_para.transpower);
		sendValueToDwin(eSUPERFICIAL_TRANSPOWER_VP, Getmode_para.transpower);
		break;
		////////////////////////////////////////////////////////////////////////////////////
	case RTC_DAY_VP:
		rtcData.day = rec.touchData;
		break;
	case RTC_MONTH_VP:
		rtcData.month = rec.touchData;
		break;
	case RTC_YEAR_VP:
		rtcData.year = rec.touchData;
		break;
	case RTC_HOURS_VP:
		rtcData.hours = rec.touchData;
		break;
	case RTC_MIN_VP:
		rtcData.min = rec.touchData;
		break;
	case RTC_SEC_VP:
		rtcData.sec = rec.touchData;
		break;

	case RTC_ENTER_VP:
		RTC_Enter(touchData);
		break;
		//	case RTC_SETTINGS_VP: pageSwitch(_pageIndex) break;

	case CONTROL_HUB_VP:
		controllhub(touchData);
		break;

	case WAKETOUCH_VP:
		wakeSystem();
		break;
	case RTC_HOME_VP:
		homeTriger();
		break;

	case PSW_TEXT_VP:
		break;
	case RTC_TEXT_BOX_DATE_VP:
		break;

	case RTC_TEXT_BOX_TIME_VP:
		break;

	case PSW_TEXT_BOX_VP:
		break;

	case LOADING_PAGE_VP:
		break;

	}
}

// Generic safe update function
uint8_t DWIN_UI::safeUpdate(uint8_t newVal, uint8_t prevVal) {
	int diff = newVal - prevVal;

	if (diff == 1 || diff == -1 || diff == 0) {
		// Accept only +/-1 changes
		prevVal = newVal;
		return newVal;
	} else {
		// Reject jump, keep previous
		return prevVal;
	}
}

void DWIN_UI::controllhub(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		/*left motor*/
		break;
	case 0x0002:
		/*right motor*/
		break;
	case 0x0003:
		/*up*/
		break;
	case 0x0004:
		/*down*/
		break;
	case 0x0005:
		/*on*/

		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint16_t )1200 - 1);
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
		break;
	case 0x0006:

		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint16_t )0);
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
		/*off*/
		break;
	case 0x0007:
		HAL_GPIO_WritePin(WATER_OUT_GPIO_Port, WATER_OUT_Pin, GPIO_PIN_SET);
		/*open*/
		break;
	case 0x0008:
		HAL_GPIO_WritePin(WATER_OUT_GPIO_Port, WATER_OUT_Pin, GPIO_PIN_RESET);
		/*close*/
		break;
	case 0x0009:
		pageSwitch(RTC_SETTING_PAGE);
		//		controlHub_Back();
		break;
	case 0x0010:
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
		HAL_GPIO_WritePin(WATER_OUT_GPIO_Port, WATER_OUT_Pin, GPIO_PIN_RESET);
		homeTriger();

		break;
	}
}

void DWIN_UI::wakeSystem(void) {
	pageSwitch(GYNO_IDLE_VP);

}
void DWIN_UI::SleepMode(void) {
	pageSwitch(LOADING_PAGE);
	sendValueToDwin(LOADING_PAGE_VP, 0);
}
void DWIN_UI::LodingPage(void) {
	pageSwitch(LOADING_PAGE);

	sendValueToDwin(LOADING_PAGE_VP, 1);

}
void DWIN_UI::RTC_Enter(uint16_t touchData) {

	sysMain.sysProcess.state = SysMain::UPDATE_RTC;
	pageSwitch(MODE_PAGE);

}
void DWIN_UI::Mode_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(GYNO_SELECTION_PAGE);
		sysMain.sysProcess.setmode = SysMain::GYNO;
		break;
	case 0x0002:
		pageSwitch(FINENEEDLE_SELECTION_PAGE);
		sysMain.sysProcess.setmode = SysMain::FINNEEDEL;
		break;
	case 0x0003:
		pageSwitch(MUCOID_SELECTION_PAGE);
		sysMain.sysProcess.setmode = SysMain::MCUSPEC;
		break;
	case 0x0004:
		pageSwitch(BODYFLUID_SELECTION_PAGE);
		sysMain.sysProcess.setmode = SysMain::BODYFULID;
		break;
	case 0x0005:
		pageSwitch(ECT_SELECTION_PAGE);
		sysMain.sysProcess.setmode = SysMain::ECTUSER;
		break;
	case 0x0006:
		pageSwitch(SUPERFICIAL_SELECTION_PAGE);
		sysMain.sysProcess.setmode = SysMain::BRUSHSCRAPING;
		break;
	case 0x0007:
		homeTriger();
		break;

	}
}
void DWIN_UI::gyno_Home_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(GYNO_PROCESS_PAUSE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_START;
		break;
	case 0x0002:
		pageSwitch(MODE_PAGE);
		break;
	case 0x0003:
		pageSwitch(CONTROL_HUB_PAGE);
		break;/*RTC control hub*/
	case 0x0004:
		break;

	}
}
void DWIN_UI::gyno_process_on_Going_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(GYNO_START_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_STOP;
		break;
	}
}
void DWIN_UI::gyno_stop_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(GYNO_PROCESS_PAUSE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_STOP_RUN;
		break;
	case 0x0002:
		pageSwitch(GYNO_IDLE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_RESET;
		break;

	}
}
void DWIN_UI::gyno_selection_Page(uint16_t touchData) {

	switch (touchData) {

	case 0x0001:
		pageSwitch(GYNO_IDLE_PAGE);
		sysMain.sysProcess.mode = SysMain::GYNO;
		break;
	case 0x0002:
		sysMain.sysProcess.setmode = SysMain::GYNO;
		pageSwitch(PASSWORD_PAGE);
		sendStringToDwin(PSW_TEXT_VP, "____");
		//		sendValueToDwin(GYNO_VCTIME_VP,
		//				Getmode_para.vctime =
		//						sysMain.CurrentDataUpdate.modes[SysMain::GYNO].vctime);
		//		sendValueToDwin(GYNO_VCPER_VP,
		//				Getmode_para.vcpress =
		//						sysMain.CurrentDataUpdate.modes[SysMain::GYNO].vcper);
		//		sendValueToDwin(GYNO_TRANSHOLD_VP,
		//				Getmode_para.transhold =
		//						sysMain.CurrentDataUpdate.modes[SysMain::GYNO].transhold);
		//		sendValueToDwin(GYNO_TRANSPOWER_VP,
		//				Getmode_para.transpower =
		//						sysMain.CurrentDataUpdate.modes[SysMain::GYNO].transpower);
		//		sendValueToDwin(eGYNO_VCTIME_VP,
		//				Getmode_para.vctime =
		//						sysMain.CurrentDataUpdate.modes[SysMain::GYNO].vctime);
		//		sendValueToDwin(eGYNO_VCPER_VP,
		//				Getmode_para.vcpress =
		//						sysMain.CurrentDataUpdate.modes[SysMain::GYNO].vcper);
		//		sendValueToDwin(eGYNO_TRANSHOLD_VP,
		//				Getmode_para.transhold =
		//						sysMain.CurrentDataUpdate.modes[SysMain::GYNO].transhold);
		//		sendValueToDwin(eGYNO_TRANSPOWER_VP,
		//				Getmode_para.transpower =
		//						sysMain.CurrentDataUpdate.modes[SysMain::GYNO].transpower);
		//
		//		prev_vctime = Getmode_para.vctime;
		//		prev_vcpress = Getmode_para.vcpress;
		//		prev_transhold = Getmode_para.transhold;
		//		prev_transpower = Getmode_para.transpower;
		//		pageSwitch(GYNO_PARA_PAGE);

		break;
	case 0x0003:
		homeTriger();
		break;
	}

}
void DWIN_UI::gyno_settings_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(GYNO_IDLE_PAGE);
		sysMain.sysProcess.state = SysMain::UPDATE_MODES;
		sysMain.sysProcess.mode = SysMain::GYNO;
		sysMain.CurrentDataUpdate.modes[SysMain::GYNO].vctime =
				Getmode_para.vctime;
		sysMain.CurrentDataUpdate.modes[SysMain::GYNO].vcper =
				Getmode_para.vcpress;
		sysMain.CurrentDataUpdate.modes[SysMain::GYNO].transhold =
				Getmode_para.transhold;
		sysMain.CurrentDataUpdate.modes[SysMain::GYNO].transpower =
				Getmode_para.transpower;
		break;
	case 0x0002:
		sendValueToDwin(GYNO_VCTIME_VP,
				Getmode_para.vctime =
						sysMain.DefaultModeData.modes[SysMain::GYNO].vctime);
		sendValueToDwin(GYNO_VCPER_VP,
				Getmode_para.vcpress =
						sysMain.DefaultModeData.modes[SysMain::GYNO].vcper);
		sendValueToDwin(GYNO_TRANSHOLD_VP,
				Getmode_para.transhold =
						sysMain.DefaultModeData.modes[SysMain::GYNO].transhold);
		sendValueToDwin(GYNO_TRANSPOWER_VP,
				Getmode_para.transpower =
						sysMain.DefaultModeData.modes[SysMain::GYNO].transpower);
		sendValueToDwin(eGYNO_VCTIME_VP,
				Getmode_para.vctime =
						sysMain.DefaultModeData.modes[SysMain::GYNO].vctime);
		sendValueToDwin(eGYNO_VCPER_VP,
				Getmode_para.vcpress =
						sysMain.DefaultModeData.modes[SysMain::GYNO].vcper);
		sendValueToDwin(eGYNO_TRANSHOLD_VP,
				Getmode_para.transhold =
						sysMain.DefaultModeData.modes[SysMain::GYNO].transhold);
		sendValueToDwin(eGYNO_TRANSPOWER_VP,
				Getmode_para.transpower =
						sysMain.DefaultModeData.modes[SysMain::GYNO].transpower);
		prev_vctime = Getmode_para.vctime;
		prev_vcpress = Getmode_para.vcpress;
		prev_transhold = Getmode_para.transhold;
		prev_transpower = Getmode_para.transpower;

		break;
	case 0x0003:
		pageSwitch(GYNO_SELECTION_PAGE);
		break;
	}

}

void DWIN_UI::Fine_Needle_Home_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(FINENEEDLE_PROCESS_PAUSE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_START;
		sysMain.sysProcess.mode = SysMain::FINNEEDEL;
		break;
	case 0x0002:
		pageSwitch(MODE_PAGE);
		break;
	case 0x0003:
		pageSwitch(CONTROL_HUB_PAGE);
		;
		break;
	case 0x0004:
		//		pageSwitch(39)/*LOG-.1*/
		break;
	}
}
void DWIN_UI::Fine_Needle_process_on_Going_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(FINENEEDLE_START_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_STOP;
		break;
	}
}
void DWIN_UI::Fine_Needle_stop_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(FINENEEDLE_PROCESS_PAUSE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_STOP_RUN;
		break;
	case 0x0002:
		pageSwitch(FINENEEDLE_IDLE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_RESET;
		break;
	}
}
void DWIN_UI::Fine_Needle_selection_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(FINENEEDLE_IDLE_PAGE);
		sysMain.sysProcess.mode = SysMain::FINNEEDEL;
		break;
	case 0x0002:
		sysMain.sysProcess.setmode = SysMain::FINNEEDEL;
				pageSwitch(PASSWORD_PAGE);
				sendStringToDwin(PSW_TEXT_VP, "____");
//		sendValueToDwin(FINENEEDLE_VCTIME_VP,
//				Getmode_para.vctime =
//						sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].vctime);
//		sendValueToDwin(FINENEEDLE_VCPER_VP,
//				Getmode_para.vcpress =
//						sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].vcper);
//		sendValueToDwin(FINENEEDLE_TRANSHOLD_VP, Getmode_para.transhold =
//				sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].transhold);
//		sendValueToDwin(FINENEEDLE_TRANSPOWER_VP, Getmode_para.transpower =
//				sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].transpower);
//		sendValueToDwin(eFINENEEDLE_VCTIME_VP,
//				Getmode_para.vctime =
//						sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].vctime);
//		sendValueToDwin(eFINENEEDLE_VCPER_VP,
//				Getmode_para.vcpress =
//						sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].vcper);
//		sendValueToDwin(eFINENEEDLE_TRANSHOLD_VP, Getmode_para.transhold =
//				sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].transhold);
//		sendValueToDwin(eFINENEEDLE_TRANSPOWER_VP, Getmode_para.transpower =
//				sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].transpower);
//		prev_vctime = Getmode_para.vctime;
//		prev_vcpress = Getmode_para.vcpress;
//		prev_transhold = Getmode_para.transhold;
//		prev_transpower = Getmode_para.transpower;
//		pageSwitch(FINENEEDLE_PARA_PAGE);
		break;
	case 0x0003:
		homeTriger();
		break;
	}
}
void DWIN_UI::Fine_Needle_settings_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(FINENEEDLE_IDLE_PAGE);
		sysMain.sysProcess.state = SysMain::UPDATE_MODES;
		sysMain.sysProcess.mode = SysMain::FINNEEDEL;
		sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].vctime =
				Getmode_para.vctime;
		sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].vcper =
				Getmode_para.vcpress;
		sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].transhold =
				Getmode_para.transhold;
		sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].transpower =
				Getmode_para.transpower;

		break;
	case 0x0002:
		sendValueToDwin(FINENEEDLE_VCTIME_VP,
				Getmode_para.vctime =
						sysMain.DefaultModeData.modes[SysMain::FINNEEDEL].vctime);
		sendValueToDwin(FINENEEDLE_VCPER_VP,
				Getmode_para.vcpress =
						sysMain.DefaultModeData.modes[SysMain::FINNEEDEL].vcper);
		sendValueToDwin(FINENEEDLE_TRANSHOLD_VP, Getmode_para.transhold =
				sysMain.DefaultModeData.modes[SysMain::FINNEEDEL].transhold);
		sendValueToDwin(FINENEEDLE_TRANSPOWER_VP, Getmode_para.transpower =
				sysMain.DefaultModeData.modes[SysMain::FINNEEDEL].transpower);

		sendValueToDwin(eFINENEEDLE_VCTIME_VP,
				Getmode_para.vctime =
						sysMain.DefaultModeData.modes[SysMain::FINNEEDEL].vctime);
		sendValueToDwin(eFINENEEDLE_VCPER_VP,
				Getmode_para.vcpress =
						sysMain.DefaultModeData.modes[SysMain::FINNEEDEL].vcper);
		sendValueToDwin(eFINENEEDLE_TRANSHOLD_VP, Getmode_para.transhold =
				sysMain.DefaultModeData.modes[SysMain::FINNEEDEL].transhold);
		sendValueToDwin(eFINENEEDLE_TRANSPOWER_VP, Getmode_para.transpower =
				sysMain.DefaultModeData.modes[SysMain::FINNEEDEL].transpower);
		prev_vctime = Getmode_para.vctime;
		prev_vcpress = Getmode_para.vcpress;
		prev_transhold = Getmode_para.transhold;
		prev_transpower = Getmode_para.transpower;
		break;
	case 0x0003:
		pageSwitch(FINENEEDLE_SELECTION_PAGE);
		break;
	}
}

void DWIN_UI::Mucoid_Home_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(MUCOID_PROCESS_PAUSE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_START;
		sysMain.sysProcess.mode = SysMain::MCUSPEC;
		break;
	case 0x0002:
		pageSwitch(MODE_PAGE);
		break;
	case 0x0003:
		pageSwitch(CONTROL_HUB_PAGE);
		;
		break;
	case 0x0004:
		break;
	}
}
void DWIN_UI::Mucoid_process_on_Going_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(MUCOID_START_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_STOP;
		break;
	}

}
void DWIN_UI::Mucoid_stop_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(MUCOID_PROCESS_PAUSE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_STOP_RUN;
		break;
	case 0x0002:
		pageSwitch(MUCOID_IDLE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_RESET;
		break;
	}
}
void DWIN_UI::Mucoid_selection_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(MUCOID_IDLE_PAGE);
		sysMain.sysProcess.mode = SysMain::MCUSPEC;
		break;
	case 0x0002:
		sysMain.sysProcess.setmode = SysMain::MCUSPEC;
				pageSwitch(PASSWORD_PAGE);
				sendStringToDwin(PSW_TEXT_VP, "____");
//		sendValueToDwin(MUCOID_VCTIME_VP,
//				Getmode_para.vctime =
//						sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].vctime);
//		sendValueToDwin(MUCOID_VCPER_VP,
//				Getmode_para.vcpress =
//						sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].vcper);
//		sendValueToDwin(MUCOID_TRANSHOLD_VP,
//				Getmode_para.transhold =
//						sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].transhold);
//		sendValueToDwin(MUCOID_TRANSPOWER_VP,
//				Getmode_para.transpower =
//						sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].transpower);
//
//		sendValueToDwin(eMUCOID_VCTIME_VP,
//				Getmode_para.vctime =
//						sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].vctime);
//		sendValueToDwin(eMUCOID_VCPER_VP,
//				Getmode_para.vcpress =
//						sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].vcper);
//		sendValueToDwin(eMUCOID_TRANSHOLD_VP,
//				Getmode_para.transhold =
//						sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].transhold);
//		sendValueToDwin(eMUCOID_TRANSPOWER_VP,
//				Getmode_para.transpower =
//						sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].transpower);
//		prev_vctime = Getmode_para.vctime;
//		prev_vcpress = Getmode_para.vcpress;
//		prev_transhold = Getmode_para.transhold;
//		prev_transpower = Getmode_para.transpower;
//		pageSwitch(MUCOID_PARA_PAGE);

		break;
	case 0x0003:
		homeTriger();
		break;
	}
}
void DWIN_UI::Mucoid_settings_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(MUCOID_IDLE_PAGE);
		sysMain.sysProcess.state = SysMain::UPDATE_MODES;
		sysMain.sysProcess.mode = SysMain::MCUSPEC;
		sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].vctime =
				Getmode_para.vctime;
		sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].vcper =
				Getmode_para.vcpress;
		sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].transhold =
				Getmode_para.transhold;
		sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].transpower =
				Getmode_para.transpower;
		break;
	case 0x0002:
		sendValueToDwin(MUCOID_VCTIME_VP,
				Getmode_para.vctime =
						sysMain.DefaultModeData.modes[SysMain::MCUSPEC].vctime);
		sendValueToDwin(MUCOID_VCPER_VP,
				Getmode_para.vcpress =
						sysMain.DefaultModeData.modes[SysMain::MCUSPEC].vcper);
		sendValueToDwin(MUCOID_TRANSHOLD_VP,
				Getmode_para.transhold =
						sysMain.DefaultModeData.modes[SysMain::MCUSPEC].transhold);
		sendValueToDwin(MUCOID_TRANSPOWER_VP,
				Getmode_para.transpower =
						sysMain.DefaultModeData.modes[SysMain::MCUSPEC].transpower);

		sendValueToDwin(eMUCOID_VCTIME_VP,
				Getmode_para.vctime =
						sysMain.DefaultModeData.modes[SysMain::MCUSPEC].vctime);
		sendValueToDwin(eMUCOID_VCPER_VP,
				Getmode_para.vcpress =
						sysMain.DefaultModeData.modes[SysMain::MCUSPEC].vcper);
		sendValueToDwin(eMUCOID_TRANSHOLD_VP,
				Getmode_para.transhold =
						sysMain.DefaultModeData.modes[SysMain::MCUSPEC].transhold);
		sendValueToDwin(eMUCOID_TRANSPOWER_VP,
				Getmode_para.transpower =
						sysMain.DefaultModeData.modes[SysMain::MCUSPEC].transpower);
		prev_vctime = Getmode_para.vctime;
		prev_vcpress = Getmode_para.vcpress;
		prev_transhold = Getmode_para.transhold;
		prev_transpower = Getmode_para.transpower;
		break;
	case 0x0003:
		pageSwitch(MUCOID_SELECTION_PAGE);
		break;
	}
}

void DWIN_UI::fluid_Home_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(BODYFLUID_PROCESS_PAUSE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_START;
		sysMain.sysProcess.mode = SysMain::BODYFULID;
		break;
	case 0x0002:
		pageSwitch(MODE_PAGE);
		break;
	case 0x0003:
		pageSwitch(CONTROL_HUB_PAGE);
		;
		break;
	case 0x0004:
		break;
	}
}
void DWIN_UI::fluid_process_on_Going_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(BODYFLUID_START_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_STOP;
		break;
	}

}
void DWIN_UI::fluid_stop_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(BODYFLUID_PROCESS_PAUSE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_STOP_RUN;
		break;
	case 0x0002:
		pageSwitch(BODYFLUID_IDLE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_RESET;
		break;
	}
}
void DWIN_UI::fluid_selection_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(BODYFLUID_IDLE_PAGE);
		sysMain.sysProcess.mode = SysMain::BODYFULID;
		break;
	case 0x0002:
		sysMain.sysProcess.setmode = SysMain::BODYFULID;
					pageSwitch(PASSWORD_PAGE);
					sendStringToDwin(PSW_TEXT_VP, "____");
//		sendValueToDwin(BODYFLUID_VCTIME_VP,
//				Getmode_para.vctime =
//						sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].vctime);
//		sendValueToDwin(BODYFLUID_VCPER_VP,
//				Getmode_para.vcpress =
//						sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].vcper);
//		sendValueToDwin(BODYFLUID_TRANSHOLD_VP,
//				Getmode_para.transhold =
//						sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].transhold);
//		sendValueToDwin(BODYFLUID_TRANSPOWER_VP, Getmode_para.transpower =
//				sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].transpower);
//
//		sendValueToDwin(eBODYFLUID_VCTIME_VP,
//				Getmode_para.vctime =
//						sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].vctime);
//		sendValueToDwin(eBODYFLUID_VCPER_VP,
//				Getmode_para.vcpress =
//						sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].vcper);
//		sendValueToDwin(eBODYFLUID_TRANSHOLD_VP, Getmode_para.transhold =
//				sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].transhold);
//		sendValueToDwin(eBODYFLUID_TRANSPOWER_VP, Getmode_para.transpower =
//				sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].transpower);
//		prev_vctime = Getmode_para.vctime;
//		prev_vcpress = Getmode_para.vcpress;
//		prev_transhold = Getmode_para.transhold;
//		prev_transpower = Getmode_para.transpower;
//		pageSwitch(BODYFLUID_PARA_PAGE);

		break;
	case 0x0003:
		homeTriger();
		break;
	}
}
void DWIN_UI::fluid_settings_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(BODYFLUID_IDLE_PAGE);
		sysMain.sysProcess.state = SysMain::UPDATE_MODES;
		sysMain.sysProcess.mode = SysMain::BODYFULID;
		sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].vctime =
				Getmode_para.vctime;
		sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].vcper =
				Getmode_para.vcpress;
		sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].transhold =
				Getmode_para.transhold;
		sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].transpower =
				Getmode_para.transpower;

		break;
	case 0x0002:
		sendValueToDwin(BODYFLUID_VCTIME_VP,
				Getmode_para.vctime =
						sysMain.DefaultModeData.modes[SysMain::BODYFULID].vctime);
		sendValueToDwin(BODYFLUID_VCPER_VP,
				Getmode_para.vcpress =
						sysMain.DefaultModeData.modes[SysMain::BODYFULID].vcper);
		sendValueToDwin(BODYFLUID_TRANSHOLD_VP,
				Getmode_para.transhold =
						sysMain.DefaultModeData.modes[SysMain::BODYFULID].transhold);
		sendValueToDwin(BODYFLUID_TRANSPOWER_VP, Getmode_para.transpower =
				sysMain.DefaultModeData.modes[SysMain::BODYFULID].transpower);
		sendValueToDwin(eBODYFLUID_VCTIME_VP,
				Getmode_para.vctime =
						sysMain.DefaultModeData.modes[SysMain::BODYFULID].vctime);
		sendValueToDwin(eBODYFLUID_VCPER_VP,
				Getmode_para.vcpress =
						sysMain.DefaultModeData.modes[SysMain::BODYFULID].vcper);
		sendValueToDwin(eBODYFLUID_TRANSHOLD_VP, Getmode_para.transhold =
				sysMain.DefaultModeData.modes[SysMain::BODYFULID].transhold);
		sendValueToDwin(eBODYFLUID_TRANSPOWER_VP, Getmode_para.transpower =
				sysMain.DefaultModeData.modes[SysMain::BODYFULID].transpower);
		prev_vctime = Getmode_para.vctime;
		prev_vcpress = Getmode_para.vcpress;
		prev_transhold = Getmode_para.transhold;
		prev_transpower = Getmode_para.transpower;
		break;
	case 0x0003:
		pageSwitch(BODYFLUID_SELECTION_PAGE);
		break;
	}
}

void DWIN_UI::ECT_Home_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(ECT_PROCESS_PAUSE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_START;
		sysMain.sysProcess.mode = SysMain::ECTUSER;
		break;
	case 0x0002:
		pageSwitch(MODE_PAGE);
		break;
	case 0x0003:
		pageSwitch(CONTROL_HUB_PAGE);
		break;
	case 0x0004:
		break;
	}
}
void DWIN_UI::ECT_process_on_Going_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(ECT_START_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_STOP;
		break;
	}

}
void DWIN_UI::ECT_stop_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(ECT_PROCESS_PAUSE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_STOP_RUN;
		break;
	case 0x0002:
		pageSwitch(ECT_IDLE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_RESET;
		break;
	}
}
void DWIN_UI::ECT_selection_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(ECT_IDLE_PAGE);

		sysMain.sysProcess.mode = SysMain::ECTUSER;
		break;
	case 0x0002:
		sysMain.sysProcess.setmode = SysMain::ECTUSER;
					pageSwitch(PASSWORD_PAGE);
					sendStringToDwin(PSW_TEXT_VP, "____");
//		sendValueToDwin(ECT_VCTIME_VP,
//				Getmode_para.vctime =
//						sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].vctime);
//		sendValueToDwin(ECT_VCPER_VP,
//				Getmode_para.vcpress =
//						sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].vcper);
//		sendValueToDwin(ECT_TRANSHOLD_VP,
//				Getmode_para.transhold =
//						sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].transhold);
//		sendValueToDwin(ECT_TRANSPOWER_VP,
//				Getmode_para.transpower =
//						sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].transpower);
//
//		sendValueToDwin(eECT_VCTIME_VP,
//				Getmode_para.vctime =
//						sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].vctime);
//		sendValueToDwin(eECT_VCPER_VP,
//				Getmode_para.vcpress =
//						sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].vcper);
//		sendValueToDwin(eECT_TRANSHOLD_VP,
//				Getmode_para.transhold =
//						sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].transhold);
//		sendValueToDwin(eECT_TRANSPOWER_VP,
//				Getmode_para.transpower =
//						sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].transpower);
//
//		prev_vctime = Getmode_para.vctime;
//		prev_vcpress = Getmode_para.vcpress;
//		prev_transhold = Getmode_para.transhold;
//		prev_transpower = Getmode_para.transpower;
//
//		pageSwitch(ECT_PARA_PAGE);
		break;
	case 0x0003:
		homeTriger();
		break;
	}
}
void DWIN_UI::ECT_settings_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(ECT_IDLE_PAGE);
		sysMain.sysProcess.state = SysMain::UPDATE_MODES;
		sysMain.sysProcess.mode = SysMain::ECTUSER;
		sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].vctime =
				Getmode_para.vctime;
		sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].vcper =
				Getmode_para.vcpress;
		sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].transhold =
				Getmode_para.transhold;
		sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].transpower =
				Getmode_para.transpower;
		break;
	case 0x0002:
		sendValueToDwin(ECT_VCTIME_VP,
				Getmode_para.vctime =
						sysMain.DefaultModeData.modes[SysMain::ECTUSER].vctime);
		sendValueToDwin(ECT_VCPER_VP,
				Getmode_para.vcpress =
						sysMain.DefaultModeData.modes[SysMain::ECTUSER].vcper);
		sendValueToDwin(ECT_TRANSHOLD_VP,
				Getmode_para.transhold =
						sysMain.DefaultModeData.modes[SysMain::ECTUSER].transhold);
		sendValueToDwin(ECT_TRANSPOWER_VP,
				Getmode_para.transpower =
						sysMain.DefaultModeData.modes[SysMain::ECTUSER].transpower);
		sendValueToDwin(eECT_VCTIME_VP,
				Getmode_para.vctime =
						sysMain.DefaultModeData.modes[SysMain::ECTUSER].vctime);
		sendValueToDwin(eECT_VCPER_VP,
				Getmode_para.vcpress =
						sysMain.DefaultModeData.modes[SysMain::ECTUSER].vcper);
		sendValueToDwin(eECT_TRANSHOLD_VP,
				Getmode_para.transhold =
						sysMain.DefaultModeData.modes[SysMain::ECTUSER].transhold);
		sendValueToDwin(eECT_TRANSPOWER_VP,
				Getmode_para.transpower =
						sysMain.DefaultModeData.modes[SysMain::ECTUSER].transpower);

		prev_vctime = Getmode_para.vctime;
		prev_vcpress = Getmode_para.vcpress;
		prev_transhold = Getmode_para.transhold;
		prev_transpower = Getmode_para.transpower;

		break;
	case 0x0003:
		pageSwitch(ECT_SELECTION_PAGE);
		break;
	}
}

void DWIN_UI::superficial_Home_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(SUPERFICIAL_PROCESS_PAUSE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_START;
		sysMain.sysProcess.mode = SysMain::BRUSHSCRAPING;
		break;
	case 0x0002:
		pageSwitch(MODE_PAGE);
		break;
	case 0x0003:
		pageSwitch(CONTROL_HUB_PAGE);
		break;
	case 0x0004:
		break;
	}
}
void DWIN_UI::superficial_process_on_Going_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(SUPERFICIAL_START_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_STOP;
		break;
	}

}
void DWIN_UI::superficial_stop_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(SUPERFICIAL_PROCESS_PAUSE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_STOP_RUN;
		break;
	case 0x0002:
		pageSwitch(SUPERFICIAL_IDLE_PAGE);
		sysMain.sysProcess.state = SysMain::PRS_RESET;
		break;
	}
}
void DWIN_UI::superficial_selection_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(SUPERFICIAL_IDLE_PAGE);
		sysMain.sysProcess.mode = SysMain::BRUSHSCRAPING;
		break;
	case 0x0002:
		sysMain.sysProcess.setmode = SysMain::BRUSHSCRAPING;
					pageSwitch(PASSWORD_PAGE);
					sendStringToDwin(PSW_TEXT_VP, "____");
//		sendValueToDwin(SUPERFICIAL_VCTIME_VP,
//				Getmode_para.vctime =
//						sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].vctime);
//		sendValueToDwin(SUPERFICIAL_VCPER_VP,
//				Getmode_para.vcpress =
//						sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].vcper);
//		sendValueToDwin(SUPERFICIAL_TRANSHOLD_VP,
//				Getmode_para.transhold =
//						sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].transhold);
//		sendValueToDwin(SUPERFICIAL_TRANSPOWER_VP,
//				Getmode_para.transpower =
//						sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].transpower);
//
//		sendValueToDwin(eSUPERFICIAL_VCTIME_VP,
//				Getmode_para.vctime =
//						sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].vctime);
//		sendValueToDwin(eSUPERFICIAL_VCPER_VP,
//				Getmode_para.vcpress =
//						sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].vcper);
//		sendValueToDwin(eSUPERFICIAL_TRANSHOLD_VP,
//				Getmode_para.transhold =
//						sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].transhold);
//		sendValueToDwin(eSUPERFICIAL_TRANSPOWER_VP,
//				Getmode_para.transpower =
//						sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].transpower);
//
//		prev_vctime = Getmode_para.vctime;
//		prev_vcpress = Getmode_para.vcpress;
//		prev_transhold = Getmode_para.transhold;
//		prev_transpower = Getmode_para.transpower;
//
//		pageSwitch(SUPERFICIAL_PARA_PAGE);
		break;
	case 0x0003:
		homeTriger();
		break;
	}
}
void DWIN_UI::superficial_settings_Page(uint16_t touchData) {
	switch (touchData) {
	case 0x0001:
		pageSwitch(SUPERFICIAL_IDLE_PAGE);
		sysMain.sysProcess.state = SysMain::UPDATE_MODES;
		sysMain.sysProcess.mode = SysMain::BRUSHSCRAPING;
		sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].vctime =
				Getmode_para.vctime;
		sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].vcper =
				Getmode_para.vcpress;
		sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].transhold =
				Getmode_para.transhold;
		sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].transpower =
				Getmode_para.transpower;
		break;
	case 0x0002:
		sendValueToDwin(SUPERFICIAL_VCTIME_VP,
				Getmode_para.vctime =
						sysMain.DefaultModeData.modes[SysMain::BRUSHSCRAPING].vctime);
		sendValueToDwin(SUPERFICIAL_VCPER_VP,
				Getmode_para.vcpress =
						sysMain.DefaultModeData.modes[SysMain::BRUSHSCRAPING].vcper);
		sendValueToDwin(SUPERFICIAL_TRANSHOLD_VP,
				Getmode_para.transhold =
						sysMain.DefaultModeData.modes[SysMain::BRUSHSCRAPING].transhold);
		sendValueToDwin(SUPERFICIAL_TRANSPOWER_VP,
				Getmode_para.transpower =
						sysMain.DefaultModeData.modes[SysMain::BRUSHSCRAPING].transpower);

		sendValueToDwin(eSUPERFICIAL_VCTIME_VP,
				Getmode_para.vctime =
						sysMain.DefaultModeData.modes[SysMain::BRUSHSCRAPING].vctime);
		sendValueToDwin(eSUPERFICIAL_VCPER_VP,
				Getmode_para.vcpress =
						sysMain.DefaultModeData.modes[SysMain::BRUSHSCRAPING].vcper);
		sendValueToDwin(eSUPERFICIAL_TRANSHOLD_VP,
				Getmode_para.transhold =
						sysMain.DefaultModeData.modes[SysMain::BRUSHSCRAPING].transhold);
		sendValueToDwin(eSUPERFICIAL_TRANSPOWER_VP,
				Getmode_para.transpower =
						sysMain.DefaultModeData.modes[SysMain::BRUSHSCRAPING].transpower);

		prev_vctime = Getmode_para.vctime;
		prev_vcpress = Getmode_para.vcpress;
		prev_transhold = Getmode_para.transhold;
		prev_transpower = Getmode_para.transpower;
		break;
	case 0x0003:
		pageSwitch(SUPERFICIAL_SELECTION_PAGE);
		break;
	}
}

void DWIN_UI::homeTriger(void) {
	uint16_t pages[] = { GYNO_IDLE_PAGE, FINENEEDLE_IDLE_PAGE, MUCOID_IDLE_PAGE,
			BODYFLUID_IDLE_PAGE, ECT_IDLE_PAGE, SUPERFICIAL_IDLE_PAGE };
	uint8_t mode = sysMain.sysProcess.setmode;

	pageSwitch(pages[mode]);
}

void DWIN_UI::Psw_(uint16_t touchData) {


	    uint8_t key = touchData;

	    // digit 0-9
	    if (key <= 9)
	    {
	        if (passIndex < 4)   // ✅ FIX (not 5)
	        {
	        	if(passIndex==0)
	        	{
	        		sendStringToDwin(PSW_TEXT_VP, "____ ");
	        	}
	            passwordBuffer[passIndex++] = '0' + key;
	            passwordBuffer[passIndex] = '\0';

	            // FIXED LENGTH = 4 chars + null
	            char starText[5];

	            for (uint8_t i = 0; i < 4; i++)   // ✅ FIX (not 5)
	            {
	                if (i < passIndex)
	                    starText[i] = '*';
	                else
	                    starText[i] = '_';
	            }
	            starText[4] = '\0';  // correct

	            sendStringToDwin(PSW_TEXT_VP, starText);
	        }
	        else
	        {
	            passIndex = 0;
	            passwordBuffer[0] = '\0';
	            sendStringToDwin(PSW_TEXT_VP, "____ ");
	        }
	    }

	    // ENTER
	    else if (key == 16)
	    {
	        if (passIndex == 4)
	        {
	            if (strcmp(passwordBuffer, "4512") == 0)
	            {
	                passwordOK = 1;
	                Psw_enter_button();
	            }
	            else
	            {
	                passwordOK = 0;
	                sendStringToDwin(PSW_TEXT_VP, "WRONG");
	            }
	        }
	        else
	        {
	            sendStringToDwin(PSW_TEXT_VP, "____ ");
	        }

	        passIndex = 0;
	        passwordBuffer[0] = '\0';
	    }

	    // BACK
	    else if (key == 17)
	    {
	        Psw_back_button();

	        passIndex = 0;
	        passwordBuffer[0] = '\0';

	        sendStringToDwin(PSW_TEXT_VP, "____ ");
	    }
	}


void DWIN_UI::Psw_enter_button() {
	uint16_t addresses[][4] =
			{ { GYNO_VCTIME_VP, GYNO_VCPER_VP, GYNO_TRANSHOLD_VP,
					GYNO_TRANSPOWER_VP }, 								// GYNO
					{ FINENEEDLE_VCTIME_VP, FINENEEDLE_VCPER_VP,
							FINENEEDLE_TRANSHOLD_VP, FINENEEDLE_TRANSPOWER_VP }, // FINNEEDEL
					{ MUCOID_VCTIME_VP, MUCOID_VCPER_VP, MUCOID_TRANSHOLD_VP,
							MUCOID_TRANSPOWER_VP }, 				// MCUSPEC
					{ BODYFLUID_VCTIME_VP, BODYFLUID_VCPER_VP,
							BODYFLUID_TRANSHOLD_VP, BODYFLUID_TRANSPOWER_VP }, // BODYFULID
					{ ECT_VCTIME_VP, ECT_VCPER_VP, ECT_TRANSHOLD_VP,
							ECT_TRANSPOWER_VP }, 					// ECTUSER
					{ SUPERFICIAL_VCTIME_VP, SUPERFICIAL_VCPER_VP,
							ECT_TRANSHOLD_VP, ECT_TRANSPOWER_VP } // BRUSHSCRAPING
			};
	uint16_t pages[] = { GYNO_PARA_PAGE, FINENEEDLE_PARA_PAGE, MUCOID_PARA_PAGE,
			BODYFLUID_PARA_PAGE, ECT_PARA_PAGE, SUPERFICIAL_PARA_PAGE };
	if(sysMain.sysProcess.setmode==SysMain::GYNO)
	{
				sendValueToDwin(GYNO_VCTIME_VP,
						Getmode_para.vctime =
								sysMain.CurrentDataUpdate.modes[SysMain::GYNO].vctime);
				sendValueToDwin(GYNO_VCPER_VP,
						Getmode_para.vcpress =
								sysMain.CurrentDataUpdate.modes[SysMain::GYNO].vcper);
				sendValueToDwin(GYNO_TRANSHOLD_VP,
						Getmode_para.transhold =
								sysMain.CurrentDataUpdate.modes[SysMain::GYNO].transhold);
				sendValueToDwin(GYNO_TRANSPOWER_VP,
						Getmode_para.transpower =
								sysMain.CurrentDataUpdate.modes[SysMain::GYNO].transpower);
				sendValueToDwin(eGYNO_VCTIME_VP,
						Getmode_para.vctime =
								sysMain.CurrentDataUpdate.modes[SysMain::GYNO].vctime);
				sendValueToDwin(eGYNO_VCPER_VP,
						Getmode_para.vcpress =
								sysMain.CurrentDataUpdate.modes[SysMain::GYNO].vcper);
				sendValueToDwin(eGYNO_TRANSHOLD_VP,
						Getmode_para.transhold =
								sysMain.CurrentDataUpdate.modes[SysMain::GYNO].transhold);
				sendValueToDwin(eGYNO_TRANSPOWER_VP,
						Getmode_para.transpower =
								sysMain.CurrentDataUpdate.modes[SysMain::GYNO].transpower);
	}
	else if(sysMain.sysProcess.setmode==SysMain::FINNEEDEL)
	{
				sendValueToDwin(FINENEEDLE_VCTIME_VP,
						Getmode_para.vctime =
								sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].vctime);
				sendValueToDwin(FINENEEDLE_VCPER_VP,
						Getmode_para.vcpress =
								sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].vcper);
				sendValueToDwin(FINENEEDLE_TRANSHOLD_VP, Getmode_para.transhold =
						sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].transhold);
				sendValueToDwin(FINENEEDLE_TRANSPOWER_VP, Getmode_para.transpower =
						sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].transpower);
				sendValueToDwin(eFINENEEDLE_VCTIME_VP,
						Getmode_para.vctime =
								sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].vctime);
				sendValueToDwin(eFINENEEDLE_VCPER_VP,
						Getmode_para.vcpress =
								sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].vcper);
				sendValueToDwin(eFINENEEDLE_TRANSHOLD_VP, Getmode_para.transhold =
						sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].transhold);
				sendValueToDwin(eFINENEEDLE_TRANSPOWER_VP, Getmode_para.transpower =
						sysMain.CurrentDataUpdate.modes[SysMain::FINNEEDEL].transpower);
	}
	else if(sysMain.sysProcess.setmode==SysMain::MCUSPEC)
	{
				sendValueToDwin(MUCOID_VCTIME_VP,
						Getmode_para.vctime =
								sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].vctime);
				sendValueToDwin(MUCOID_VCPER_VP,
						Getmode_para.vcpress =
								sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].vcper);
				sendValueToDwin(MUCOID_TRANSHOLD_VP,
						Getmode_para.transhold =
								sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].transhold);
				sendValueToDwin(MUCOID_TRANSPOWER_VP,
						Getmode_para.transpower =
								sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].transpower);

				sendValueToDwin(eMUCOID_VCTIME_VP,
						Getmode_para.vctime =
								sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].vctime);
				sendValueToDwin(eMUCOID_VCPER_VP,
						Getmode_para.vcpress =
								sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].vcper);
				sendValueToDwin(eMUCOID_TRANSHOLD_VP,
						Getmode_para.transhold =
								sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].transhold);
				sendValueToDwin(eMUCOID_TRANSPOWER_VP,
						Getmode_para.transpower =
								sysMain.CurrentDataUpdate.modes[SysMain::MCUSPEC].transpower);
	}
	else if(sysMain.sysProcess.setmode==SysMain::BODYFULID)
	{
			sendValueToDwin(BODYFLUID_VCTIME_VP,
					Getmode_para.vctime =
							sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].vctime);
			sendValueToDwin(BODYFLUID_VCPER_VP,
					Getmode_para.vcpress =
							sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].vcper);
			sendValueToDwin(BODYFLUID_TRANSHOLD_VP,
					Getmode_para.transhold =
							sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].transhold);
			sendValueToDwin(BODYFLUID_TRANSPOWER_VP, Getmode_para.transpower =
					sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].transpower);

			sendValueToDwin(eBODYFLUID_VCTIME_VP,
					Getmode_para.vctime =
							sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].vctime);
			sendValueToDwin(eBODYFLUID_VCPER_VP,
					Getmode_para.vcpress =
							sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].vcper);
			sendValueToDwin(eBODYFLUID_TRANSHOLD_VP, Getmode_para.transhold =
					sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].transhold);
			sendValueToDwin(eBODYFLUID_TRANSPOWER_VP, Getmode_para.transpower =
					sysMain.CurrentDataUpdate.modes[SysMain::BODYFULID].transpower);

	}
	else if(sysMain.sysProcess.setmode==SysMain::ECTUSER)
	{
				sendValueToDwin(ECT_VCTIME_VP,
						Getmode_para.vctime =
								sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].vctime);
				sendValueToDwin(ECT_VCPER_VP,
						Getmode_para.vcpress =
								sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].vcper);
				sendValueToDwin(ECT_TRANSHOLD_VP,
						Getmode_para.transhold =
								sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].transhold);
				sendValueToDwin(ECT_TRANSPOWER_VP,
						Getmode_para.transpower =
								sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].transpower);

				sendValueToDwin(eECT_VCTIME_VP,
						Getmode_para.vctime =
								sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].vctime);
				sendValueToDwin(eECT_VCPER_VP,
						Getmode_para.vcpress =
								sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].vcper);
				sendValueToDwin(eECT_TRANSHOLD_VP,
						Getmode_para.transhold =
								sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].transhold);
				sendValueToDwin(eECT_TRANSPOWER_VP,
						Getmode_para.transpower =
								sysMain.CurrentDataUpdate.modes[SysMain::ECTUSER].transpower);
	}
	else if(sysMain.sysProcess.setmode==SysMain::BRUSHSCRAPING)
	{
				sendValueToDwin(SUPERFICIAL_VCTIME_VP,
						Getmode_para.vctime =
								sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].vctime);
				sendValueToDwin(SUPERFICIAL_VCPER_VP,
						Getmode_para.vcpress =
								sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].vcper);
				sendValueToDwin(SUPERFICIAL_TRANSHOLD_VP,
						Getmode_para.transhold =
								sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].transhold);
				sendValueToDwin(SUPERFICIAL_TRANSPOWER_VP,
						Getmode_para.transpower =
								sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].transpower);

				sendValueToDwin(eSUPERFICIAL_VCTIME_VP,
						Getmode_para.vctime =
								sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].vctime);
				sendValueToDwin(eSUPERFICIAL_VCPER_VP,
						Getmode_para.vcpress =
								sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].vcper);
				sendValueToDwin(eSUPERFICIAL_TRANSHOLD_VP,
						Getmode_para.transhold =
								sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].transhold);
				sendValueToDwin(eSUPERFICIAL_TRANSPOWER_VP,
						Getmode_para.transpower =
								sysMain.CurrentDataUpdate.modes[SysMain::BRUSHSCRAPING].transpower);
	}

		prev_vctime = Getmode_para.vctime;
		prev_vcpress = Getmode_para.vcpress;
		prev_transhold = Getmode_para.transhold;
		prev_transpower = Getmode_para.transpower;
		pageSwitch(pages[sysMain.sysProcess.setmode]);
}

void DWIN_UI::Psw_back_button() {
	uint16_t pages[] = { GYNO_SELECTION_PAGE, FINENEEDLE_SELECTION_PAGE,
			MUCOID_SELECTION_PAGE, BODYFLUID_SELECTION_PAGE, ECT_SELECTION_PAGE,
			SUPERFICIAL_SELECTION_PAGE };
	uint8_t mode = sysMain.sysProcess.setmode;
	pageSwitch(pages[mode]);

}

void DWIN_UI::pageSwitch(uint8_t _pageIndex) {
	uint8_t dwinByteArray[10] = { 0X5A, 0XA5, 0X07, 0X82, 0X00, 0X84, 0X5A,
			0X01, 0X00, _pageIndex };
	HAL_UART_Transmit(&huart3, dwinByteArray, 10, 10);
	HAL_Delay(10);
	uint8_t _dwinByteArray[10] = { 0X5A, 0XA5, 0X07, 0X82, 0X00, 0X84, 0X5A,
			0X01, 0X00, _pageIndex };
	HAL_UART_Transmit(&huart3, _dwinByteArray, 10, 100);

}

void DWIN_UI::sendValueToDwin(uint16_t _vpAddress, uint16_t _vpValue) {
	uint8_t vpAddressHighByte = _vpAddress / 256;
	uint8_t vpAddressLowByte = _vpAddress % 256;
	uint8_t vpValueHighByte = _vpValue / 256;
	uint8_t vpValueLowByte = _vpValue % 256;
	uint8_t dwinByteArray[8] = { 0X5A, 0XA5, 0X05, 0X82, vpAddressHighByte,
			vpAddressLowByte, vpValueHighByte, vpValueLowByte };
	HAL_UART_Transmit(&huart3, dwinByteArray, sizeof(dwinByteArray), 100);

}

void DWIN_UI::sendFloatValueToDwin(uint16_t _vpAddress, float _vpValue) {
	//	uint8_t vpAddressHighByte = _vpAddress / 256;
	//	uint8_t vpAddressLowByte = _vpAddress % 256;
	//	uint8_t *_floatData = (uint8_t*) &_vpValue;
	//
	//	uint8_t dwinByteArray[10] = { 0X5A, 0XA5, 0X07, 0X82, vpAddressHighByte,
	//			vpAddressLowByte, _floatData[3], _floatData[2], _floatData[1],
	//			_floatData[0] };

}

void DWIN_UI::sendStringToDwin(uint16_t _vpAddress, const char *str) {
    uint8_t vpAddressHighByte = _vpAddress >> 8;
    uint8_t vpAddressLowByte  = _vpAddress & 0xFF;

    uint8_t strLength = strlen(str);

    uint8_t frameSize = 6 + strLength;   // ✅ FIX (not 5)

    uint8_t dwinByteArray[32]; // safer static buffer (avoid malloc in MCU)

    dwinByteArray[0] = 0x5A;
    dwinByteArray[1] = 0xA5;
    dwinByteArray[2] = strLength + 3;   // data length
    dwinByteArray[3] = 0x82;
    dwinByteArray[4] = vpAddressHighByte;
    dwinByteArray[5] = vpAddressLowByte;

    memcpy(&dwinByteArray[6], str, strLength);

    HAL_UART_Transmit(&huart3, dwinByteArray, frameSize, 100);
}

void DWIN_UI::sendCurrentDateTimeToDwin() {
	char dateStr[20];
	char timeStr[20];
	// Format date as "DD-MM-YYYY\n\r"
	//	sprintf(dateStr, "%02d-%02d-%04d\r",
	//			rtc._time.dayofmonth,
	//			rtc._time.month,
	//			2000 + rtc._time.year);
	snprintf(dateStr, sizeof(dateStr), "%02d-%02d-%04d\r", rtc._time.dayofmonth,
			rtc._time.month, 2000 + rtc._time.year);
	// Format time as "HH-MM-SS\n\r"
	//	sprintf(timeStr, "%02d :%02d :%02d\r",
	//			rtc._time.hour,
	//			rtc._time.minutes,
	//			rtc._time.seconds);
	snprintf(timeStr, sizeof(timeStr), "%02d :%02d :%02d\r", rtc._time.hour,
			rtc._time.minutes, rtc._time.seconds);
	// Send formatted date and time to the DWIN display
	sendStringToDwin(RTC_TEXT_BOX_DATE_VP, dateStr);
	sendStringToDwin(RTC_TEXT_BOX_TIME_VP, timeStr);
}

void DWIN_UI::restartHMI() {
	//    uint8_t cmd[] = {0x5A,0xA5,0x05,0x82,0x00,0x84,0x5A,0x00};
	//    HAL_UART_Transmit(&huart3, cmd, sizeof(cmd), 100);
	//	uint8_t sendBuffer[] = {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x04, 0x55, 0xaa, 0x5a ,0xa5};
	//	HAL_UART_Transmit(&huart3, sendBuffer,10, 100);

}

