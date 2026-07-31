/*
 * uart_parse.h
 *
 *  Created on: 16-Jun-2026
 *      Author: richa
 */

#ifndef INC_UART_PARSE_H_
#define INC_UART_PARSE_H_

#include "main.h"

#define START_BYTE 0xFD
#define SLAVE_ID 0x32
#define END_BYTE 0xFE

extern uint8_t uart_rx_status,uart_tx_status;
extern uint8_t i2c_tx_status,i2c_rx_status;
extern uint32_t uart_instance;

typedef struct __attribute__((packed)){
    uint8_t  startByte;//253
    uint8_t  dataLenth;
    float  volt;
    float  current;
    uint8_t  cmd;
    uint8_t end_byte;
}channel_t;

typedef union __attribute__((packed)){
	channel_t data;
	uint8_t buffer[sizeof(channel_t)];
}u_channel_t;

extern u_channel_t channel_1;
extern u_channel_t channel_2;
extern u_channel_t channel_3;
extern u_channel_t channel_4;
extern u_channel_t channel_5;
extern u_channel_t channel_6;

typedef struct __attribute__((packed)){
    uint8_t  startByte;//253
    uint8_t  slaveID;//32
    uint8_t  dataLenth;
    uint8_t  subSlave1;//0x01
    float  volt_1;
    float  current_1;
    uint8_t  cmd_1;
    uint8_t  subSlave2;//0x02
    float  volt_2;
    float  current_2;
    uint8_t  cmd_2;
    uint8_t  subSlave3;//0x03
    float  volt_3;
    float  current_3;
    uint8_t  cmd_3;
    uint8_t  subSlave4;//0x04
    float  volt_4;
    float  current_4;
    uint8_t  cmd_4;
    uint8_t  subSlave5;//0x05
    float  volt_5;
    float  current_5;
    uint8_t  cmd_5;
    uint8_t  subSlave6;//0x06
    float  volt_6;
    float  current_6;
    uint8_t  cmd_6;
    uint8_t  mStatus;//0x01 enable, 0x02 disable
    uint8_t  endByte;//254

}UART_Packet_t;

#define FRAME_LENGTH ((sizeof(UART_Packet_t))-4)

#define FRAME_STRUCTURE_LENGTH (sizeof(UART_Packet_t))

typedef union
{
    UART_Packet_t packet;
    uint8_t bytes[sizeof(UART_Packet_t)];
}UART_Data_t;


extern UART_Data_t txData;

void UART_Read_Init(void);

void UART_Process_Byte(uint8_t byte);

#endif /* INC_UART_PARSE_H_ */
