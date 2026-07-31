/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEADTIME_US            2500U
#define HALF_CYCLE_US          (10000U - DEADTIME_US)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
typedef struct __attribute__((packed)){
	uint8_t  startByte;//253
	uint8_t  dataLenth;
	uint8_t  volt;
	uint8_t  cmd;
	uint8_t end_byte;
}channel_t;

typedef union __attribute__((packed)){
	channel_t data;
	uint8_t buffer[sizeof(channel_t)];
}u_channel_t;

u_channel_t rx_channel;
u_channel_t tx_channel;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void us50Hzgen(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define ADC_BUF_LEN 1
uint16_t adc_buf[ADC_BUF_LEN];
uint16_t new_pulse_value=700;
uint16_t pre_pulse_value;
uint16_t i,pulseSeq;
uint32_t timertiks,leddelay;
uint8_t resiveflag;
uint16_t adc_ch1 = 0;
uint16_t adc_ch2 = 0;

void Update_TIM3_Period(uint32_t new_period);
void delay_us (uint16_t us);
void ADC_ReadChannels(void);

typedef struct
{
	uint16_t volt;
	uint16_t period;
	uint16_t pulse;
} BoostCal_t;

const BoostCal_t BoostTable[] =
{
		{10, 600, 120},
		{20, 600, 140},
		{30, 600, 177},
		{40, 600, 220},
		{50, 600, 240},
		{60, 635, 280},
		{70, 670, 360},
		{80, 800, 380},
		{90, 800, 393},
		{100,800, 460}
};

#define BOOST_TABLE_SIZE (sizeof(BoostTable)/sizeof(BoostTable[0]))
void Boost_SetVoltage(uint8_t voltage);
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

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM1_Init();
	MX_TIM3_Init();
	MX_ADC1_Init();
	MX_TIM16_Init();
	MX_I2C1_Init();
	/* USER CODE BEGIN 2 */
	HAL_GPIO_WritePin(AC50Hz1_GPIO_Port, AC50Hz1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AC50Hz2_GPIO_Port, AC50Hz2_Pin, GPIO_PIN_RESET);

	hi2c1.Init.OwnAddress1 = (0x26<< 1);
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	HAL_I2C_Init(&hi2c1);

	HAL_TIM_Base_Start(&htim3);               // Start TIM3 to generate TRGO
	Update_TIM3_Period(0);
	HAL_TIM_OnePulse_Start(&htim1, TIM_CHANNEL_1); // Arm TIM1 for one pulse
	//	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, i);
	HAL_TIM_Base_Start(&htim16);
	HAL_Delay(1500);
	HAL_I2C_Slave_Receive_IT(&hi2c1,rx_channel.buffer,sizeof(rx_channel.buffer));
	timertiks=HAL_GetTick();
	leddelay=HAL_GetTick();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		ADC_ReadChannels();
		tx_channel.data.volt= ((float)adc_ch2 * 100.0f) / 3220.0f;

		tx_channel.data.startByte= 253;
		tx_channel.data.end_byte= 254;
		tx_channel.data.dataLenth= 2;

		if(pulseSeq!=0)
		{
			us50Hzgen();
		}
		else
		{
			HAL_GPIO_WritePin(AC50Hz1_GPIO_Port, AC50Hz1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(AC50Hz2_GPIO_Port, AC50Hz2_Pin, GPIO_PIN_RESET);
			tx_channel.data.cmd= 0;
		}


		if((HAL_GetTick()-leddelay)>=200)
		{
			leddelay=HAL_GetTick();
			if(resiveflag==1)
			{
				resiveflag=0;
				timertiks=HAL_GetTick();
				HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
			}
		}
		if(rx_channel.data.cmd==1)
		{
			pulseSeq=rx_channel.data.volt;
			tx_channel.data.cmd=1;
		}
		else
		{
			pulseSeq=0;
			tx_channel.data.cmd= 0;
		}
		if((HAL_GetTick()-timertiks)>=5000)
		{
			pulseSeq=0;
			tx_channel.data.cmd= 0;
			timertiks=HAL_GetTick();
		    HAL_I2C_DeInit(&hi2c1);
		    __HAL_I2C_CLEAR_FLAG(&hi2c1, I2C_FLAG_BERR);
		    __HAL_I2C_CLEAR_FLAG(&hi2c1, I2C_FLAG_ARLO);
		    __HAL_I2C_CLEAR_FLAG(&hi2c1, I2C_FLAG_OVR);
		    __HAL_I2C_CLEAR_FLAG(&hi2c1, I2C_FLAG_STOPF);
		    __HAL_I2C_CLEAR_FLAG(&hi2c1, I2C_FLAG_AF);
		    HAL_I2C_Init(&hi2c1);
			HAL_I2C_Slave_Receive_IT(&hi2c1,rx_channel.buffer,sizeof(rx_channel.buffer));
		}
		Boost_SetVoltage(pulseSeq);



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
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
	RCC_OscInitStruct.PLL.PLLN = 8;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void us50Hzgen(void)
{
	HAL_GPIO_WritePin(AC50Hz1_GPIO_Port, AC50Hz1_Pin, GPIO_PIN_RESET);
	delay_us(DEADTIME_US);

	HAL_GPIO_WritePin(AC50Hz2_GPIO_Port, AC50Hz2_Pin, GPIO_PIN_SET);
	delay_us(HALF_CYCLE_US);

	HAL_GPIO_WritePin(AC50Hz2_GPIO_Port, AC50Hz2_Pin, GPIO_PIN_RESET);
	delay_us(DEADTIME_US);

	HAL_GPIO_WritePin(AC50Hz1_GPIO_Port, AC50Hz1_Pin, GPIO_PIN_SET);
	delay_us(HALF_CYCLE_US);
}
void Update_TIM3_Period(uint32_t new_period)
{
	__HAL_TIM_SET_AUTORELOAD(&htim3, new_period);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	HAL_TIM_GenerateEvent(&htim3, TIM_EVENTSOURCE_UPDATE);
}
void delay_us (uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim16,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim16) < us);  // wait for the counter to reach the us input in the parameter
}

void ADC_ReadChannels(void)
{
	HAL_ADC_Start(&hadc1);

	// Read Channel 1 (Rank 1)
	if(HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
	{
		adc_ch1 = HAL_ADC_GetValue(&hadc1);
	}

	// Read Channel 2 (Rank 2)
	if(HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
	{
		adc_ch2 = HAL_ADC_GetValue(&hadc1);
	}

	HAL_ADC_Stop(&hadc1);
}
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance == I2C1)
	{
		if(rx_channel.data.startByte == 253 &&rx_channel.data.end_byte == 254)
		{
			uint8_t voltage = rx_channel.data.volt;

			uint8_t cmd = rx_channel.data.cmd;
			resiveflag=1;
			// Process the received data here
		}
//		HAL_I2C_Slave_Receive_IT(hi2c,rx_channel.buffer,sizeof(rx_channel.buffer));
        HAL_I2C_Slave_Transmit_IT(hi2c,tx_channel.buffer,sizeof(tx_channel.buffer));

	}
}
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        HAL_I2C_Slave_Receive_IT(hi2c,rx_channel.buffer,sizeof(rx_channel.buffer));
    }
}
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        HAL_I2C_DeInit(&hi2c1);
        HAL_I2C_Init(&hi2c1);
        HAL_I2C_Slave_Receive_IT(hi2c,
                                 rx_channel.buffer,
                                 sizeof(rx_channel.buffer));
    }
}
void Boost_SetVoltage(uint8_t voltage)
{
	if(voltage==0)
	{
		HAL_GPIO_WritePin(AC50Hz1_GPIO_Port, AC50Hz1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(AC50Hz2_GPIO_Port, AC50Hz2_Pin, GPIO_PIN_RESET);
	}
	if(voltage < 10) voltage = 10;
	if(voltage > 100) voltage = 100;

	uint8_t index = (voltage - 10) / 10;

	if(index >= 9)
	{
		Update_TIM3_Period(BoostTable[9].period);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, BoostTable[9].pulse);
		return;
	}

	uint8_t rem = (voltage - BoostTable[index].volt);

	uint16_t period =
			BoostTable[index].period +
			(rem * (BoostTable[index+1].period - BoostTable[index].period)) / 10;

	uint16_t pulse =
			BoostTable[index].pulse +
			(rem * (BoostTable[index+1].pulse - BoostTable[index].pulse)) / 10;

	Update_TIM3_Period(period);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse);
}
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
