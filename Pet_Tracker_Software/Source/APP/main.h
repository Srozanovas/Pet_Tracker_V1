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
extern uint16_t epo_file_status; 
extern uint16_t pet_tracker_options;
extern uint8_t pet_tracker_apn[50];
extern uint8_t gnss_status; 
extern char latitude[20];
extern char longitude[20];
extern char time_of_fix[20];
extern uint8_t num_of_fixes;
extern uint8_t allowed_contacts[10][20];
extern uint8_t acce_status;
extern uint8_t acce_options;
extern float acceleration;
extern float average_acceleration;
extern uint8_t pet_tracker_mode; 
extern uint16_t normal_mode_time;
extern uint16_t power_save_mode_time;
extern uint8_t pet_tracker_mode;

#define EPO_DOWNLOAD_LINK "http://wepodownload.mediatek.com/EPO_GPS_3_1.DAT"

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



//PET TRACKER STATUS FLAGS

#define GNSS_POWER_ON  		  (1U<<0)
#define MODEM_POWER_ON		  (1U<<1)
#define LED_POWER_ON		    (1U<<2)
#define ACCE_POWER_ON 		  (1U<<3)
#define REGISTERED_TO_NET	  (1U<<4)
#define V4_POWER_ON			    (1U<<5)
#define WAITING_FOR_MESSAGE (1U<<6)
#define MODEM_INITIALISED   (1U<<7)
#define TRACKER_ON          (1U<<8)


//PET_TRACKER_OPTIONS 

#define AGPS_ENABLE       (1U<<0)
#define DEBUG_ENABLE      (1U<<1)
#define FULL_DEBUG        (1U<<2)
#define GNSS_FULL_PARSE   (1U<<3)


//ACCE flags
#define ACCE_ENABLE     (1U<<0)
#define ACCE_DATA_READY (1U<<1)
#define ACCE_CALIBRATED (1U<<2)

#define ACCE_USE_INTERUPT (1U<<0)

//GNSS STATUS FLAGS 

#define GNSS_FIX_COMPLETE       (1U<<0)
#define EPO_DOWNLOAD_COMPLETE   (1U<<1)


#define PET_TRACKER_MODE_NORMAL 0
#define PET_TRACKER_MODE_POWER_SAVE 1

#define EEPROM_OPTIONS   eEepromAT24C256A0
#define EEPROM_GPS       eEepromAT24C256A1

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
