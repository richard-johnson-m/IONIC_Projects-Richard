/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define S_CS_Pin GPIO_PIN_3
#define S_CS_GPIO_Port GPIOA
#define L_CS_Pin GPIO_PIN_4
#define L_CS_GPIO_Port GPIOA
#define L_SLEEP_Pin GPIO_PIN_4
#define L_SLEEP_GPIO_Port GPIOC
#define L_RESET_Pin GPIO_PIN_5
#define L_RESET_GPIO_Port GPIOC
#define L_STEP_Pin GPIO_PIN_0
#define L_STEP_GPIO_Port GPIOB
#define L_DIR_Pin GPIO_PIN_1
#define L_DIR_GPIO_Port GPIOB
#define L_BIN1_Pin GPIO_PIN_2
#define L_BIN1_GPIO_Port GPIOB
#define L_BIN2_Pin GPIO_PIN_7
#define L_BIN2_GPIO_Port GPIOE
#define L_FAULT_Pin GPIO_PIN_8
#define L_FAULT_GPIO_Port GPIOE
#define L_STALL_Pin GPIO_PIN_9
#define L_STALL_GPIO_Port GPIOE
#define S_SLEEP_Pin GPIO_PIN_10
#define S_SLEEP_GPIO_Port GPIOE
#define S_RESET_Pin GPIO_PIN_11
#define S_RESET_GPIO_Port GPIOE
#define S_DIR_Pin GPIO_PIN_12
#define S_DIR_GPIO_Port GPIOE
#define S_STEP_Pin GPIO_PIN_13
#define S_STEP_GPIO_Port GPIOE
#define S_BIN1_Pin GPIO_PIN_14
#define S_BIN1_GPIO_Port GPIOE
#define S_BIN2_Pin GPIO_PIN_15
#define S_BIN2_GPIO_Port GPIOE
#define S_FAULT_Pin GPIO_PIN_10
#define S_FAULT_GPIO_Port GPIOB
#define SD_INPUT_Pin GPIO_PIN_0
#define SD_INPUT_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
