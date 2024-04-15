/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#define DEBUGO_KODAS

//status flags 
extern uint32_t gpio_init_status;
extern uint32_t gpio_pin_level_status;
extern uint16_t  uart_init_status;
extern uint16_t pet_tracker_init_status;
extern uint32_t pet_tracker_status;
extern uint16_t threads_status;




//UART INIT FLAGS 
#define DEBUG_UART_DRIVER_INIT  (1U<<0)
#define DEBUG_UART_MUTEX_INIT   (1U<<1)  
#define MODEM_UART_DRIVER_INIT  (1U<<2)
#define MODEM_UART_MUTEX_INIT   (1U<<3) 
#define DEBUG_MESSAGE_QUEUE_INIT  (1U<<4)
#define MODEM_MESSAGE_QUEUE_INIT  (1U<<5)
#define UART_FLAGS_INIT         (1U<<6)
#define DEBUG_UART_ENABLED	    (1U<<7)
#define MODEM_UART_ENABLED	    (1U<<8)

//PET TRACKER INIT FLAGS 
#define CMD_API_INITIALISED     (1U<<0)
#define CLI_APP_INITIALISED     (1U<<1)


//THREADS STATUS 
#define UART_API_THREAD_INIT    (1U<<0)
#define UART_API_THREAD_STOPPED (1U<<1)



//MESSAGE QUEUES PARAMS
#define COMMAND_QUEUE_SIZE 10
#define COMMANDS_QUEUE_PUT_TIMEOUT 100
#define MODULE_MAX_NAME 20
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

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
