/* USER CODE BEGIN Header */
///**
// ******************************************************************************
// * @file           : main.c
// * @brief          : Main program body
// ******************************************************************************
// * @attention
// *
// * Copyright (c) 2026 STMicroelectronics.
// * All rights reserved.
// *
// * This software is licensed under terms that can be found in the LICENSE file
// * in the root directory of this software component.
// * If no LICENSE file comes with this software, it is provided AS-IS.
// *
// ******************************************************************************
// */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usb_host.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mDRV_Config.h"
//#include "usb_log.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//#define DRV_CS_LOW()   HAL_GPIO_WritePin(L_CS_GPIO_Port, L_CS_Pin, GPIO_PIN_RESET)
//#define DRV_CS_HIGH()  HAL_GPIO_WritePin(L_CS_GPIO_Port, L_CS_Pin, GPIO_PIN_SET)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//uint8_t txBuf[2];
//uint8_t rxBuf[2];
//
////uint8_t tx[2];
////   uint8_t rx[2];
//
//uint16_t tx;
//uint16_t rx;
//
//GPIO_PinState fault_state;
ApplicationTypeDef ret;
extern ApplicationTypeDef Appli_state;
//HAL_StatusTypeDef i2c_status;
////uint8_t tx[2] = {0x00,0x00};
//
//uint32_t spi_error;
//uint32_t count = 0;
//GPIO_PinState sleep_state;
//GPIO_PinState reset_state;
//GPIO_PinState cs_state;
//uint16_t duty = 0;
//uint8_t tx_data[3];
//uint8_t mem_addr[2] = {0x00, 0x00};
//uint8_t read_data = 0;
//uint8_t limit_reached=0;
//RTC_TimeTypeDef getTime;
//RTC_DateTypeDef getDate;
//uint8_t rtc_sec;
//uint8_t usb_rx_buffer[64];
//char usb_string[64];
//uint32_t usb_rx_length;
uint8_t fault_state;
uint16_t ctrl_reg,status_reg;
uint16_t status_value = 0;
uint16_t ctrl_value = 0;
uint8_t motor_running = 0;
//
uint8_t drv_status;
uint8_t drv_cont;

uint16_t ctrl_readback;
uint8_t stattt=0;
uint8_t enbl, rdir, rstep, mode, exstall, isgain, dtime;
uint32_t fre;
volatile uint32_t debug_tim3_psc;
volatile uint32_t debug_tim3_arr;
volatile uint32_t debug_tim3_ccr3;
volatile uint32_t debug_tim3_cnt;

volatile uint32_t debug_pclk1;
volatile uint32_t debug_sysclk;

 uint8_t sd_ins;
//uint8_t spi_buf_rx[3]={0};
//uint16_t dataOut;
// volatile uint16_t test_write_val = 0x0FF;
//  volatile uint16_t test_read_val  = 0;
//  volatile uint8_t  spi_test_pass  = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */
//
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void delay_us(uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim6, 0);

	while (__HAL_TIM_GET_COUNTER(&htim6) < us);
}

#define TIM3_CLK 60000000UL
void Stepper_SetFrequency_L(uint32_t freq)
{
    uint32_t timer_clk = 84000000UL;
    uint32_t counter_clk;
    uint32_t arr;

    if (freq == 0)
    {
        TIM3->CCR3 = 0;
        return;
    }

    counter_clk = timer_clk / (TIM3->PSC + 1);

    arr = (counter_clk / freq) - 1;

    if (arr > 0xFFFF)
        arr = 0xFFFF;

    if (arr < 1)
        arr = 1;

    TIM3->ARR = arr;

    TIM3->CCR3 = (arr + 1) / 2;

    TIM3->CNT = 0;

    TIM3->EGR = TIM_EGR_UG;
}
void Stepper_SetFrequency_S(uint32_t freq)
{
	uint32_t arr;

	if (freq == 0)
	{
		TIM1->CCR3 = 0;
		return;
	}

	// TIM1 on APB2 → 84 MHz
	arr = (84000UL / freq) - 1;

	if (arr > 0xFFFF) arr = 0xFFFF;
	if (arr < 1) arr = 1;

	TIM1->ARR  = arr;
	TIM1->CCR3 = (arr + 1) / 2;   // 50% duty
	TIM1->CNT  = 0;

	TIM1->EGR = TIM_EGR_UG;
}
//void Stepper_SetFrequency_S(uint32_t frequency)
//{
//    uint32_t period;
//
//    if (frequency == 0)
//        return;
//
//    period = 1000000UL / frequency;
//
//    __HAL_TIM_SET_AUTORELOAD(&htim3, period - 1);
//
//    __HAL_TIM_SET_COMPARE(&htim3,
//                          TIM_CHANNEL_3,
//                          period / 2);
//
//    __HAL_TIM_SET_COUNTER(&htim3, 0);
//}
#define STEPS_PER_REV 1600

void SetRPM(uint16_t rpm)
{
	uint32_t freq;

	freq = ((uint32_t)rpm * STEPS_PER_REV) / 60;

	Stepper_SetFrequency(freq);
}
//// Add this function to your main.c (replace the existing one)
void DRV8711_WriteReg(uint8_t reg, uint16_t data,uint8_t stepper)
{
	uint16_t cmd;
	uint8_t tx[2];
	GPIO_TypeDef* port;
	uint16_t pin;
	cmd = ((reg & 0x07) << 12) | (data & 0x0FFF);

	tx[0] = (cmd >> 8) & 0xFF;
	tx[1] = cmd & 0xFF;

	switch(stepper){
	case 0:
	port=L_CS_GPIO_Port;
	 pin =L_CS_Pin;
	 break;
	case 1:
		 port=S_CS_GPIO_Port;
		 pin =S_CS_Pin;
    break;
	}
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
	HAL_Delay(1);

	HAL_SPI_Transmit(&hspi1, tx, 2, 100);
	HAL_Delay(1);
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}
uint16_t DRV8711_ReadReg(uint8_t reg,uint8_t stepper)
{
	uint16_t cmd;
	uint16_t data;
	uint8_t tx[2];
	uint8_t rx[2];
	GPIO_TypeDef* port;
	uint16_t pin;
	cmd = 0x8000 | ((reg & 0x07) << 12);

	tx[0] = (cmd >> 8) & 0xFF;
	tx[1] = cmd & 0xFF;
	switch(stepper){
		case 0:
		port=L_CS_GPIO_Port;
		 pin =L_CS_Pin;
		 break;
		case 1:
			 port=S_CS_GPIO_Port;
			 pin =S_CS_Pin;
	    break;
		}
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);

	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, 100);

	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);

	data = ((uint16_t)rx[0] << 8) | rx[1];

	return (data & 0x0FFF);
}


uint16_t ctrl,stall;
uint32_t freq;
uint32_t curr;

volatile uint8_t sd_raw = 99;
volatile uint32_t sd_count = 0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	//
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	//
	//
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	//
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM6_Init();
  MX_RTC_Init();
  MX_FATFS_Init();
  MX_USB_HOST_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

//	HAL_TIM_Base_Start(&htim6);
//	HAL_GPIO_WritePin(L_RESET_GPIO_Port, L_RESET_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(L_SLEEP_GPIO_Port, L_SLEEP_Pin, GPIO_PIN_SET);
//
//	HAL_GPIO_WritePin(S_RESET_GPIO_Port, S_RESET_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(S_SLEEP_GPIO_Port, S_SLEEP_Pin, GPIO_PIN_SET);
//
//	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3 );
//	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3 );
//
//
//	HAL_GPIO_WritePin(L_SLEEP_GPIO_Port, L_SLEEP_Pin, GPIO_PIN_RESET);
//	HAL_Delay(1000);
//
//	DRV8711_WriteReg(0x00, 0x0C29,0);
//	DRV8711_WriteReg(0x01, 0x0743,0);//current
//	DRV8711_WriteReg(0x02, 0x00C8,0);
//	DRV8711_WriteReg(0x03, 0x0128,0);
//	DRV8711_WriteReg(0x04, 0x0510,0);
//	DRV8711_WriteReg(0x05, 0x000A,0);
//	DRV8711_WriteReg(0x06, 0x0BAA,0);
//	DRV8711_WriteReg(0x07,0x00,0);
//
//	HAL_GPIO_WritePin(L_SLEEP_GPIO_Port, L_SLEEP_Pin, GPIO_PIN_SET);
//	HAL_Delay(1000);
//	DRV8711_WriteReg(0x07,0x00,0);
//
//
//	HAL_Delay(500);
//
//	HAL_GPIO_WritePin(S_SLEEP_GPIO_Port, S_SLEEP_Pin, GPIO_PIN_RESET);
//	HAL_Delay(1000);
//
//	DRV8711_WriteReg(0x00, 0x0C29,1);
//	DRV8711_WriteReg(0x01, 0x070A,1);//current
//	DRV8711_WriteReg(0x02, 0x00C8,1);
//	DRV8711_WriteReg(0x03, 0x0128,1);
//	DRV8711_WriteReg(0x04, 0x0510,1);
//	DRV8711_WriteReg(0x05, 0x000A,1);
//	DRV8711_WriteReg(0x06, 0x0BAA,1);
//	DRV8711_WriteReg(0x07,0x00,1);
//
//	HAL_GPIO_WritePin(S_SLEEP_GPIO_Port, S_SLEEP_Pin, GPIO_PIN_SET);
//	HAL_Delay(1000);
//	DRV8711_WriteReg(0x07,0x00,1);
//	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
//
//	Stepper_SetFrequency_L(3200);
//
//	debug_tim3_psc  = TIM3->PSC;
//	debug_tim3_arr  = TIM3->ARR;
//	debug_tim3_ccr3 = TIM3->CCR3;
//	debug_tim3_cnt  = TIM3->CNT;
//

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	//
	while (1)
	{



//		for( freq = 1; freq <= 3200; freq++)
//		{
//			Stepper_SetFrequency_L(freq);
//						Stepper_SetFrequency_S(freq);
//						curr=DRV8711_ReadReg(0x01 ,0);
//			//			stall=HAL_GPIO_ReadPin(L_STALL_GPIO_Port, L_STALL_Pin);
//			HAL_Delay(5);
//		}
//		HAL_Delay(5000);
//
//		for( freq = 3200; freq > 1; freq--)
//		{
//			Stepper_SetFrequency_L(freq);
//						Stepper_SetFrequency_S(freq);
//						curr=DRV8711_ReadReg(0x01 ,0);
//			//			stall=HAL_GPIO_ReadPin(L_STALL_GPIO_Port, L_STALL_Pin);
//			HAL_Delay(5);
//		}

		//
		//		ctrl_readback = DRV8711_ReadReg(0x00);
		//
		//		enbl    = (ctrl_readback >> 0)  & 0x0;   // 1 bit
		//		rdir    = (ctrl_readback >> 1)  & 0x1;   // 1 bit
		//		rstep   = (ctrl_readback >> 2)  & 0x1;   // 1 bit
		//		mode    = (ctrl_readback >> 3)  & 0xF;   // 4 bits (bits 6:3)
		//		exstall = (ctrl_readback >> 7)  & 0x1;   // 1 bit
		//		isgain  = (ctrl_readback >> 8)  & 0x3;   // 2 bits (bits 9:8)
		//		dtime   = (ctrl_readback >> 10) & 0x3;   // 2 bits (bits 11:10)
		//		fault_state=HAL_GPIO_ReadPin(L_FAULT_GPIO_Port, L_FAULT_Pin);
		//        status_value=DRV8711_ReadReg(0x07);
		//		   HAL_GPIO_WritePin(L_STEP_GPIO_Port, L_STEP_Pin, GPIO_PIN_SET);
		//		    HAL_Delay(1);   // pulse high time
		//		    HAL_GPIO_WritePin(L_STEP_GPIO_Port, L_STEP_Pin, GPIO_PIN_RESET);
		//		    HAL_Delay(1);
		//		stall=HAL_GPIO_ReadPin(L_STALL_GPIO_Port, L_STALL_Pin);
		//		    HAL_GPIO_WritePin(L_STEP_GPIO_Port, L_STEP_Pin, GPIO_PIN_SET);
		//		    		    HAL_Delay(1);
		//		    HAL_GPIO_WritePin(L_STEP_GPIO_Port, L_STEP_Pin, GPIO_PIN_RESET);
		//		    		    HAL_Delay(1);
		//
		// Wait between steps - THIS controls motor speed
		// Try different values:
		//		    HAL_Delay(10);
		//
				 if(ret != FR_OK)
						    {
						        if(Appli_state == APPLICATION_READY)
						        {
						            ret = USB_CSV_Init();

						            if(ret == FR_OK)
						            {
						                USB_Get_Info();
						            }
						        }
						    }
						    else
						    {
						        USB_CSV_Write(12.5, 0.8);

						        HAL_Delay(100);
						    }


						    MX_USB_HOST_Process();
		//
		//		//		while (1)
		//		//		{
		//		////			uint8_t msg[] = "Hello PC\r\n";
		//		////
		//		////			CDC_Transmit_FS(msg,sizeof(msg)-1);
		//		////
		//		////			HAL_Delay(1000);
		//		//
		//		//

    /* USER CODE END WHILE */
    MX_USB_HOST_Process();

    /* USER CODE BEGIN 3 */
//				    if (Appli_state == APPLICATION_READY)
//				    {
//				    	 ret=f_mount(&USBHFatFS, USBHPath, 1) ;
//				        if (ret == FR_OK)
//				        {
//				            // Open/create log.csv
//				            break;
//				        }
//				    }
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// Add this function to your code

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	//	/* User can add his own implementation to report the HAL error return state */
	//	__disable_irq();
	//	while (1)
	//	{
	//	}
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
	//	/* User can add his own implementation to report the file name and line number,
	//     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
