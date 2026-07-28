/*
 * eeprom.h
 *
 *  Created on: Dec 4, 2024
 *      Author: ASUS
 */

#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_


#include "stm32f1xx_hal.h"

// EEPROM I2C address and other constants
#define EEPROM_ADDRESS 0xA0
#define EEPROM_MAXPKT 32
#define EEPROM_WRITE 10
#define EEPROM_TIMEOUT (5 * EEPROM_WRITE)
class eeprom{
public:
	void EEPROM_Write(uint16_t address, uint8_t* data, uint16_t size);
	void EEPROM_Read(uint16_t address, uint8_t* data, uint16_t size);
};

#endif /* INC_EEPROM_H_ */
