/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f1xx_hal.h"

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
#define ST7735_CS_Pin GPIO_PIN_4
#define ST7735_CS_GPIO_Port GPIOA
#define ST7735_DC_Pin GPIO_PIN_0
#define ST7735_DC_GPIO_Port GPIOB
#define ST7735_RES_Pin GPIO_PIN_1
#define ST7735_RES_GPIO_Port GPIOB
#define final_Pin GPIO_PIN_9
#define final_GPIO_Port GPIOA
#define banheiro_agua_Pin GPIO_PIN_10
#define banheiro_agua_GPIO_Port GPIOA
#define quanti_Pin GPIO_PIN_11
#define quanti_GPIO_Port GPIOA
#define iniciar_aula_Pin GPIO_PIN_12
#define iniciar_aula_GPIO_Port GPIOA
#define led_iniciar_Pin GPIO_PIN_3
#define led_iniciar_GPIO_Port GPIOB
#define led_quanti_Pin GPIO_PIN_4
#define led_quanti_GPIO_Port GPIOB
#define led_BA_Pin GPIO_PIN_5
#define led_BA_GPIO_Port GPIOB
#define led_final_Pin GPIO_PIN_6
#define led_final_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
