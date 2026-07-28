/*
 * xRtcHand.cpp
 *
 *  Created on: Nov 14, 2024
 *      Author: ASUS
 */

#include "xRtcHand.h"

uint32_t count;
XRtcHand rtc;
// Convert normal decimal numbers to binary coded decimal
uint8_t XRtcHand::decToBcd(int val) {
    return static_cast<uint8_t>((val / 10 * 16) + (val % 10));
}

// Convert binary coded decimal to normal decimal numbers
int XRtcHand::bcdToDec(uint8_t val) {
    return static_cast<int>((val / 16 * 10) + (val % 16));
}

// Function to set time
void XRtcHand::Set_Time_n_Date(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year) {
    uint8_t set_time[7] = {
        decToBcd(sec), decToBcd(min), decToBcd(hour),
        decToBcd(dow), decToBcd(dom), decToBcd(month), decToBcd(year)
    };

    HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, 0x00, 1, set_time, 7, 1000);
}

// Function to get time
void XRtcHand::Get_Time_n_Date() {
    uint8_t get_time[7];
    HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x00, 1, get_time, 7, 1000);

    _time.seconds = bcdToDec(get_time[0]);
    _time.minutes = bcdToDec(get_time[1]);
    _time.hour = bcdToDec(get_time[2]);
    _time.dayofweek = bcdToDec(get_time[3]);
    _time.dayofmonth = bcdToDec(get_time[4]);
    _time.month = bcdToDec(get_time[5]);
    _time.year = bcdToDec(get_time[6]);
}

// Function to get temperature
float XRtcHand::Get_Temp() {
    uint8_t temp[2];
    HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x11, 1, temp, 2, 1000);
    TEMP = static_cast<float>(temp[0]) + (temp[1] >> 6) / 4.0;
    return TEMP;
}

// Force temperature conversion
void XRtcHand::force_temp_conv() {
    uint8_t status = 0, control = 0;
    HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x0F, 1, &status, 1, 100);  // Read status register
    if (status & 0x80) {
    	powerFail=1;
    }
    else{
    	powerFail=0;
    }
    if (!(status & 0x04)) {
        HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x0E, 1, &control, 1, 100);  // Read control register
        uint8_t updated_control = control | 0x20;
        HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, 0x0E, 1, &updated_control, 1, 100);
    }
}
