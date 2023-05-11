/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
<<<<<<< HEAD
  * Copyright (c) 2022 STMicroelectronics.
=======
  * Copyright (c) 2023 STMicroelectronics.
>>>>>>> e5c2739 (slow)
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
#define DBUS_Pin GPIO_PIN_11
#define DBUS_GPIO_Port GPIOC
#define Laser_Pin GPIO_PIN_8
#define Laser_GPIO_Port GPIOC
#define Heat_Power_Pin GPIO_PIN_6
#define Heat_Power_GPIO_Port GPIOF
#define CS1_Accel_Pin GPIO_PIN_4
#define CS1_Accel_GPIO_Port GPIOA
#define INT1_Accel_Pin GPIO_PIN_4
#define INT1_Accel_GPIO_Port GPIOC
#define INT1_Accel_EXTI_IRQn EXTI4_IRQn
#define INT1_Gyro_Pin GPIO_PIN_5
#define INT1_Gyro_GPIO_Port GPIOC
#define INT1_Gyro_EXTI_IRQn EXTI9_5_IRQn
#define CS1_Gyro_Pin GPIO_PIN_0
#define CS1_Gyro_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */