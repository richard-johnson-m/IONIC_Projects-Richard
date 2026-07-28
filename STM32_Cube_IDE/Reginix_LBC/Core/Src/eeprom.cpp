/*
 * eeprom.c
 *
 *  Created on: Dec 4, 2024
 *      Author: ASUS
 */




#include "eeprom.h"


extern I2C_HandleTypeDef hi2c1;


void eeprom::EEPROM_Write(uint16_t address, uint8_t* data, uint16_t size) {
    HAL_StatusTypeDef result = HAL_OK;
    uint16_t counter = 0;

    while (counter < size && result == HAL_OK) {
        uint16_t diff = size - counter;
        if (diff >= EEPROM_MAXPKT) {
            result = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, address + counter, I2C_MEMADD_SIZE_16BIT, data + counter, EEPROM_MAXPKT, EEPROM_TIMEOUT);
            counter += EEPROM_MAXPKT;
        } else {
            result = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, address + counter, I2C_MEMADD_SIZE_16BIT, data + counter, diff, EEPROM_TIMEOUT);
            counter += diff;
        }
        HAL_Delay(EEPROM_WRITE);
    }
}

void eeprom::EEPROM_Read(uint16_t address, uint8_t* data, uint16_t size) {
    HAL_StatusTypeDef result = HAL_OK;
    uint16_t counter = 0;

    while (counter < size && result == HAL_OK) {
        uint16_t diff = size - counter;
        if (diff >= EEPROM_MAXPKT) {
            result = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDRESS, address + counter, I2C_MEMADD_SIZE_16BIT, data + counter, EEPROM_MAXPKT, EEPROM_TIMEOUT);
            counter += EEPROM_MAXPKT;
        } else {
            result = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDRESS, address + counter, I2C_MEMADD_SIZE_16BIT, data + counter, diff, EEPROM_TIMEOUT);
            counter += diff;
        }
        HAL_Delay(EEPROM_WRITE / 2);
    }
}



