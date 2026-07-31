/*
 * uart_parse.c
 *
 *  Created on: 16-Jun-2026
 *      Author: richard
 */

#include "uart_parse.h"
#include <string.h>
#include "usart.h"

UART_Data_t txData;
UART_Data_t tempRx;

u_channel_t channel_1;
u_channel_t channel_2;
u_channel_t channel_3;
u_channel_t channel_4;
u_channel_t channel_5;
u_channel_t channel_6;

 uint8_t uart_rx_status,uart_tx_status;
 uint8_t i2c_tx_status,i2c_rx_status;
 uint32_t uart_instance;

void UART_Process_Byte(uint8_t byte)
{

	static uint8_t uartstate = 0;
	static uint8_t ind = 0;

	static uint8_t datalength=0;
	switch(uartstate)
	{

	case 0:
		ind = 0;

		if(byte == START_BYTE)
		{
			tempRx.bytes[ind++] = byte;
			uartstate++;
		}

		break;


	case 1:
		if(byte==SLAVE_ID){
			tempRx.bytes[ind++] = byte;
			uartstate++;
		}
		else{
			uartstate=0;
		}
		break;

	case 2:
		datalength=byte;
		if(datalength==FRAME_LENGTH){
			tempRx.bytes[ind++] = byte;
			uartstate++;
		}
		else{
			uartstate=0;
		}
		break;


	case 3:

		if(datalength!=0){
			tempRx.bytes[ind++] = byte;
			if(datalength==1){
				uartstate++;
			}
			datalength--;

		}


		break;
	case 4:

		if(byte == END_BYTE)
		{
			tempRx.bytes[ind++] = byte;
//			memcpy(&txData,&tempRx,FRAME_STRUCTURE_LENGTH);
			memcpy(&txData,
			       &tempRx,
			       ind);
			uartstate = 0;
			ind = 0;
			uart_instance=HAL_GetTick();
			uart_rx_status=1;
		}
		else
		{
			uartstate = 0;
			ind = 0;
		}

		break;

	default:

		uartstate = 0;
		ind = 0;
		break;
	}
}
