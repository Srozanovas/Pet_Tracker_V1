#include "main.h"
#include "cmsis_os.h"
#include "gpio_driver.h"
#include "uart_api.h"
#include "cli_app.h"
#include "cmd_api.h"
#include "modem_api.h"
#include "eeprom_api.h"
#include "accelerometer_api.h"
#include "cli_function_list.h"

#define  epo_link "http://wepodownload.mediatek.com/EPO_GPS_3_1.DAT"


uint8_t acce_info = 0;
uint32_t gpio_init_status = 0;
uint32_t gpio_pin_level_status = 0;
uint16_t uart_init_status = 0;
uint16_t pet_tracker_init_status = 0;
uint32_t pet_tracker_status = 0;
uint16_t threads_status = 0 ;
uint16_t epo_file_status = 0;
uint16_t pet_tracker_options = 0;
uint8_t gnss_status = 0;
uint8_t pet_tracker_apn[50];
uint8_t received_sms_ids[20] = {0};
uint8_t sms_buffer_index = 0;
char latitude[20]	={0};
char longitude[20]	={0};
char time_of_fix[20]={0};
uint8_t num_of_fixes = 0;
uint8_t allowed_contacts[10][20]={"+37067852939"};

uint8_t initialization = 1;

uint16_t x_axis;
uint16_t y_axis;
uint16_t z_axis;


osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 2000,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void StartDefaultTask(void *argument);
bool Main_APP_Pet_Tracker_Init(void);


bool Pet_Tracker_Read_Options(){
	uint8_t eeprom_save[100]={0};    //for hardcoding params into eeprom

	/*EEPROM_API_SendByte(eEepromAT24C256A1, EE_LAST_FIX, 1);
	EEPROM_API_SendBuffer(eEepromAT24C256A1, 0, "54.720251", 8);
	EEPROM_API_SendBuffer(eEepromAT24C256A1, 8, "25.277977", 8);
	EEPROM_API_SendBuffer(eEepromAT24C256A1, 16, "24/05/06,23:19:06", 17);*/
   // for (int i= 0; i<10; i++){
	//  snprintf(eeprom_save, 20, allowed_contacts[i]);
	  //  EEPROM_API_SendBuffer(eEepromAT24C256A0, EE_PET_TRACKER_PHONEBOOK10x20x8B+i*20 , eeprom_save, 20);
     // EEPROM_API_ReadBuffer(eEepromAT24C256A0, EE_PET_TRACKER_PHONEBOOK10x20x8B, eeprom_test, 20);

   // }

		//EEPROM_API_SendByte(eEepromAT24C256A1, EE_LAST_FIX, 1);
		uint8_t eeprom_read[50]={0};
	    EEPROM_API_ReadBuffer(eEepromAT24C256A0, EE_PET_TRACKER_APN_50X8B, pet_tracker_apn, 50);
	    EEPROM_API_ReadBuffer(eEepromAT24C256A0, EE_PET_TRACKER_OPTIONS_16B, eeprom_read, 2);
	    pet_tracker_options = (eeprom_read[0]<<8)|eeprom_read[1];
	    EEPROM_API_ReadByte(eEepromAT24C256A1, EE_LAST_FIX, &num_of_fixes);
		EEPROM_API_ReadBuffer(eEepromAT24C256A1, (num_of_fixes-1)*33, latitude, 8);
		EEPROM_API_ReadBuffer(eEepromAT24C256A1, (num_of_fixes-1)*33+8, longitude, 8);
		EEPROM_API_ReadBuffer(eEepromAT24C256A1, (num_of_fixes-1)*33+16, time_of_fix, 17);



}



int main(void)
{

  HAL_Init();
  SystemClock_Config();
  osKernelInitialize();

  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  if (Main_APP_Pet_Tracker_Init() != true) Error_Handler();



    osKernelStart();
    while (1){}
}

bool Main_APP_Pet_Tracker_Init(void){
	Modem_API_Init();
    UART_API_Init(eUartDebug, eBaudRate57600);
    EEPROM_API_Init();
    GPIO_Driver_Init(eGpioPinA12LEDsOn, ePinLow);
    CLI_APP_Init();
    CMD_API_ThreadInit();
    GPIO_Driver_Init(eGpioPinB0Power4V, ePinLow);
    ACC_API_Init(eAccMPU6050);
    //GPIO_Driver_Init(eGpioPinA1AcceInt, ePinLow);
    return true;

}

void Blinky(){
	GPIO_Driver_TogglePin(eGpioPinA12LEDsOn);
			osDelay(200);
			GPIO_Driver_TogglePin(eGpioPinA12LEDsOn);
			osDelay(100);
			GPIO_Driver_TogglePin(eGpioPinA12LEDsOn);

			osDelay(200);
			GPIO_Driver_TogglePin(eGpioPinA12LEDsOn);
			osDelay(100);
			GPIO_Driver_TogglePin(eGpioPinA12LEDsOn);


			osDelay(200);
			GPIO_Driver_TogglePin(eGpioPinA12LEDsOn);
			osDelay(100);
			GPIO_Driver_TogglePin(eGpioPinA12LEDsOn);

			osDelay(200);

			GPIO_Driver_TogglePin(eGpioPinA12LEDsOn);
}



uint8_t acce = 0;


void StartDefaultTask(void *argument)
{
  for(;;)
  {

	uint8_t acce_flag = osEventFlagsGet();

	//   static sms_send=0;
	//   if (initialization == 1){



	// 	//I2C_Driver_Read(eI2CAcce, 0xD0, 0x75, 1, &acce, 1);

	// 	CMD_API_PuttoQueue(eCommandModulesPower, (uint8_t)ePowerCommands4VPower, "1\n", 3);

	// 	Pet_Tracker_Read_Options();

	// 	CMD_API_PuttoQueue(eCommandModulesPower, (uint8_t)ePowerCommandsModemPower, "1\n", 3);
	// 	osDelay(20000);


	// 	Blinky();

	// 	CMD_API_PuttoQueue(eCommandModulesModem, (uint8_t)eModemCommandsSendCommand, "AT+CMGF=1\n", 50);
	// 	osDelay(2000);

	// 	Blinky();

	// 	CMD_API_PuttoQueue(eCommandModulesModem, (uint8_t)eModemCommandsGNSSPower, "1\n", 3);
	// 	osDelay(15000);

	// 	Blinky();

	// 	CMD_API_PuttoQueue(eCommandModulesModem, (uint8_t)eModemCommandsSendSMS, "+37067852939, Irenginys Veikia !\n", 50);

	// 	initialization = 0;
	// }





	// for (int  i = 0; i<5; i++){
	// 	osDelay(30000);
  	// }
	// sms_send++;


  	// CMD_API_PuttoQueue(eCommandModulesModem, eModemCommandsGetLocation, "1\n", 3);

  	// osDelay(200);


  	// if(sms_send == 2){

  	// 	  	CMD_API_PuttoQueue(eCommandModulesModem, eModemCommandSendLocation, "1\n", 3 );
  	// 	  	sms_send=0;
  	// }

}
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */


/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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

#ifdef  USE_FULL_ASSERT
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
