/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "uart_parse.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//can use rx.rx_buff_array[10] or rx.rx_buff.start_byte
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
HAL_StatusTypeDef ret1;
HAL_StatusTypeDef ret2;
HAL_StatusTypeDef ret3;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t uart_byte;
volatile uint32_t packetSize;

uint8_t i2cByte = 1;
uint8_t txdata = 0x55;
uint8_t tx[2] = {0xFD, 0x09};
uint32_t i2cTimer = 0;
channel_t testChannel;
HAL_StatusTypeDef status;
uint16_t size;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void uart_rx_i2c_tx(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */
	uint32_t ledTimer = HAL_GetTick();

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_I2C3_Init();
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */
//	HAL_UART_Receive_IT(&huart2,&uart_byte,	1);



	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
//		HAL_Delay(5);
//		uart_rx_status=1;

//		uart_rx_i2c_tx();
//HAL_GPIO_TogglePin(GPIOC, 13);
		uint32_t now = HAL_GetTick();
		 if(now - i2cTimer >= 1000)
		    {
		        i2cTimer = now;

		        i2cByte = !i2cByte;
		   	 HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		        status = HAL_I2C_Master_Transmit(&hi2c3,
		                                         0x71 << 1,
												 tx,
		                                         sizeof(tx),
		                                         1000);
		        size=sizeof(txdata);
//
//		        HAL_I2C_Master_Transmit(&hi2c3, 0x71 << 1, &txdata, 1, 1000);
//		        ret3 = HAL_I2C_Master_Transmit_IT(&hi2c3,0x71 << 1,1,1);

//		        testChannel.startByte = 0xFD;
//
//		        testChannel.dataLenth = 9;
//
//		        testChannel.volt = 12.5;
//
//		        testChannel.current = 1.25;
//
//		        testChannel.cmd = 1;
//
//		        testChannel.end_byte = 0xFE;
////
////		        uint8_t tx = 0x55;
////
////		        status = HAL_I2C_Master_Transmit(
////		                 &hi2c3,
////		                 0x71<<1,
////		                 &tx,
////		                 1,
////		                 1000);
//		        status = HAL_I2C_Master_Transmit(&hi2c3,
//		                                           0x71 << 1,
//		                                           (uint8_t *)&testChannel,
//		                                           sizeof(testChannel),
//		                                           1000);
		    }

		 if(HAL_GetTick() - ledTimer >= 400) {
			 ledTimer = HAL_GetTick();
		 HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		 }
//		uint32_t now = HAL_GetTick();
//		 if(now - i2cTimer >= 1000)
//		 {
//		     i2cTimer = now;
//
//		     testChannel.startByte = 0xFD;
//
//		     testChannel.dataLenth = 9;
//
//		     testChannel.volt = 12.5;
//
//		     testChannel.current = 1.25;
//
//		     testChannel.cmd = 1;
//
//		     testChannel.end_byte = 0xFE;
//
//		     ret3 = HAL_I2C_Master_Transmit(&hi2c3,
//		                                    0x71 << 1,
//		                                    (uint8_t *)&testChannel,
//		                                    sizeof(testChannel),
//		                                    1000);
//		 }
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
	RCC_OscInitStruct.PLL.PLLN = 85;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
//
//void uart_rx_i2c_tx(void){
//	if(uart_rx_status==1){
//
//		channel_1.data.cmd=txData.packet.cmd_1;
//		channel_2.data.cmd=txData.packet.cmd_2;
//		channel_3.data.cmd=txData.packet.cmd_3;
//		channel_4.data.cmd=txData.packet.cmd_4;
//		channel_5.data.cmd=txData.packet.cmd_5;
//		channel_6.data.cmd=txData.packet.cmd_6;
//
//		channel_1.data.volt=txData.packet.volt_1;
//		channel_2.data.volt=txData.packet.volt_2;
//		channel_3.data.volt=txData.packet.volt_3;
//		channel_4.data.volt=txData.packet.volt_4;
//		channel_5.data.volt=txData.packet.volt_5;
//		channel_6.data.volt=txData.packet.volt_6;
//
//		channel_1.data.current=txData.packet.current_1;
//		channel_2.data.current=txData.packet.current_2;
//		channel_3.data.current=txData.packet.current_3;
//		channel_4.data.current=txData.packet.current_4;
//		channel_5.data.current=txData.packet.current_5;
//		channel_6.data.current=txData.packet.current_6;
//
//		channel_1.data.startByte=START_BYTE;
//		channel_2.data.startByte=START_BYTE;
//		channel_3.data.startByte=START_BYTE;
//		channel_4.data.startByte=START_BYTE;
//		channel_5.data.startByte=START_BYTE;
//		channel_6.data.startByte=START_BYTE;
//
//		channel_1.data.end_byte=END_BYTE;
//		channel_2.data.end_byte=END_BYTE;
//		channel_3.data.end_byte=END_BYTE;
//		channel_4.data.end_byte=END_BYTE;
//		channel_5.data.end_byte=END_BYTE;
//		channel_6.data.end_byte=END_BYTE;
//
//		channel_1.data.dataLenth=9;
//		channel_2.data.dataLenth=9;
//		channel_3.data.dataLenth=9;
//		channel_4.data.dataLenth=9;
//		channel_5.data.dataLenth=9;
//		channel_6.data.dataLenth=9;
//
//		uart_rx_status=0;
//
////		 ret1 = HAL_I2C_Master_Transmit_IT(&hi2c3,0x51 << 1,1,1);
////		 ret2 = HAL_I2C_Master_Transmit_IT(&hi2c3,0x61 << 1,1,1);
////		 ret3 = HAL_I2C_Master_Transmit_IT(&hi2c3,0x71 << 1,1,1);
////		 ret1 = HAL_I2C_Master_Transmit(&hi2c3,0x51 << 1,(uint8_t*)&channel_1.data,sizeof(channel_1.data),1000);
////		 ret2 = HAL_I2C_Master_Transmit(&hi2c3,0x61 << 1,(uint8_t*)&channel_2.data,sizeof(channel_2.data),1000);
////		 ret3 = HAL_I2C_Master_Transmit(&hi2c3,0x71 << 1,(uint8_t*)&channel_3.data,sizeof(channel_3.data),1000);
//////		HAL_I2C_Master_Transmit(&hi2c1,0x52 << 1,&channel_4.buffer,sizeof(channel_4.data),1000);
////		HAL_I2C_Master_Transmit(&hi2c1,0x52 << 1,&channel_5.buffer,sizeof(channel_5.data),1000);
////		HAL_I2C_Master_Transmit(&hi2c1,0x52 << 1,&channel_6.buffer,sizeof(channel_6.data),1000);
//	}
//}
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if(huart->Instance == USART2)
//	{
//		UART_Process_Byte(uart_byte);
//
//		HAL_UART_Receive_IT(&huart2,&uart_byte,1);
//	}
//}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
