/*
 * xRtcHand.h
 *
 *  Created on: Nov 14, 2024
 *      Author: ASUS
 */

#ifndef INC_XRTCHAND_H_
#define INC_XRTCHAND_H_

#include "i2c.h"

#define DS3231_ADDRESS 0xD0

class TIME {
public:
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hour;
    uint8_t dayofweek;
    uint8_t dayofmonth;
    uint8_t month;
    uint8_t year;
};

class XRtcHand {
public:
    TIME _time;
    float TEMP;
    uint8_t powerFail;
    uint8_t decToBcd(int val);
    int bcdToDec(uint8_t val);
    void Set_Time_n_Date(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year);
    void Get_Time_n_Date();
    float Get_Temp();
    void force_temp_conv();
};
extern XRtcHand rtc;

#endif /* INC_XRTCHAND_H_ */
