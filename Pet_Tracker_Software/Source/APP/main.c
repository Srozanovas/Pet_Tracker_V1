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

//status flags 
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
uint8_t acce_options = 0;
char latitude[20]	={0};
char longitude[20]	={0};
char time_of_fix[20]={0};
uint8_t num_of_fixes = 0;
uint8_t allowed_contacts[10][20]={"+37067852939"};
uint8_t acce_status = 0;
uint8_t initialization = 1;
float acceleration = 0;
float average_acceleration = 0;
uint16_t normal_mode_counter = 0;
uint16_t power_save_mode_counter = 0;
uint8_t pet_tracker_mode = 0;
uint16_t normal_mode_time = 0;
uint16_t power_save_mode_time = 0;


#define NUMBER_OF_FIXES 1
#define NUMBER_OF_FIXES_POWER_SAVE 10


osThreadId_t main_task_handle;
const osThreadAttr_t main_task_attributes = {
  .name = "MainTask",
  .stack_size = 2000,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Main_APP_Task(void *argument);
bool Main_APP_Pet_Tracker_Init(void);
bool Pet_Tracker_Read_Options();
void Blinky();
void Pet_Tracker_NormalMode();
void Pet_Tracker_Power_Saving_Mode();
void Pet_Tracker_Start();
void Pet_Tracker_Stop();
void Pet_Tracker_ModemStart();

int main(void){
    HAL_Init();
    SystemClock_Config();
    osKernelInitialize();
    main_task_handle = osThreadNew(Main_APP_Task, NULL, &main_task_attributes);
    if(Main_APP_Pet_Tracker_Init() != true) Error_Handler();
    osKernelStart();
    while (1){}
}

bool Main_APP_Pet_Tracker_Init(void){
    GPIO_Driver_Init(eGpioPinA12LEDsOn, ePinLow);
    Modem_API_Init();
    UART_API_Init(eUartDebug, eBaudRate57600);
    EEPROM_API_Init();

    CLI_APP_Init();
    CMD_API_ThreadInit();

    //ACC_API_Init(eAccMPU6050);
    return true;

}

void Main_APP_Task(void *argument)
{
  for(;;)
  {

	//acce calibrate


    if ((initialization == 1)){
    	Pet_Tracker_Start();
	 	initialization = 0;
	 }


    //osThreadSuspend(main_task_handle);
    if (pet_tracker_mode == 0) Pet_Tracker_NormalMode();
    //else Pet_Tracker_Power_Saving_Mode();





  }
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

bool Pet_Tracker_Read_Options(){
	//uint8_t eeprom_save[100]={0};    //for hardcoding params into eeprom
		uint8_t eeprom_read[50]={0};
	    EEPROM_API_ReadBuffer(EEPROM_OPTIONS, EE_PET_TRACKER_APN_50X8B, pet_tracker_apn, 50);
	    EEPROM_API_ReadBuffer(EEPROM_OPTIONS, EE_PET_TRACKER_OPTIONS_16B, eeprom_read, 2);
	    pet_tracker_options = (eeprom_read[0]<<8)|eeprom_read[1];

	    EEPROM_API_ReadByte(EEPROM_OPTIONS, EE_PET_TRACKER_MODE_8B, &pet_tracker_mode);
	    if (pet_tracker_mode>1) pet_tracker_mode=0;

	    EEPROM_API_ReadBuffer(EEPROM_OPTIONS, EE_PET_TRACKER_FIX_FREQ_NM_16B, eeprom_read, 2);
	    normal_mode_time = (eeprom_read[1]<<8)|eeprom_read[0];

	    EEPROM_API_ReadBuffer(EEPROM_OPTIONS, EE_PET_TRACKER_FIX_FREQ_PSM_16B, eeprom_read, 2);
	    power_save_mode_time = (eeprom_read[1]<<8)|eeprom_read[0];

	    EEPROM_API_ReadByte(eEepromAT24C256A1, EE_LAST_FIX, &num_of_fixes);
		EEPROM_API_ReadBuffer(eEepromAT24C256A1, (num_of_fixes-1)*33, (uint8_t*)latitude, 8);
		EEPROM_API_ReadBuffer(eEepromAT24C256A1, (num_of_fixes-1)*33+8, (uint8_t*)longitude, 8);
		EEPROM_API_ReadBuffer(eEepromAT24C256A1, (num_of_fixes-1)*33+16, (uint8_t*)time_of_fix, 17);
}


void Pet_Tracker_NormalMode() {

    static uint8_t bad_acce = 0;
    if ((pet_tracker_status & TRACKER_ON) == 0) Pet_Tracker_ModemStart();
    osDelay(25000); //25 sec delay
    normal_mode_counter++;
    power_save_mode_counter = 0;
  
    CMD_API_PuttoQueue(eCommandModulesModem, eModemCommandsGetLocation, "1\n", 3);
    osDelay(200);

    //acce_status |= ACCE_ENABLE;
    //ACC_API_Resume();
    //osDelay(5000);
    //acce_status &= ~ACCE_ENABLE;

    //if (average_acceleration < 0.03) bad_acce++;
    //else bad_acce = 0;
    //if (bad_acce == 5){
    //CMD_API_PuttoQueue(eCommandModulesModem, eModemCommandsSendSMS, "+37067852939, No movement, sending location\n", 60);
    //bad_acce = 0;
    //normal_mode_counter = normal_mode_time;
    //osDelay(3000);
    //}

    if(normal_mode_counter >= normal_mode_time){
    CMD_API_PuttoQueue(eCommandModulesModem, eModemCommandSendLocation, "1\n", 3 );
    normal_mode_counter = 0;
    }
}


void Pet_Tracker_Power_Saving_Mode(){
    normal_mode_counter = 0;
    osDelay(30000);
    
    power_save_mode_counter++;
    if ((power_save_mode_counter < (power_save_mode_time - 3)) && ((pet_tracker_status & TRACKER_ON) != 0)) Pet_Tracker_Stop();
    if (power_save_mode_counter == power_save_mode_time-3){
        if ((pet_tracker_status & TRACKER_ON) == 0)
            Pet_Tracker_ModemStart();
        power_save_mode_counter++;
    }
    if (power_save_mode_counter >= power_save_mode_time){

        CMD_API_PuttoQueue(eCommandModulesModem, eModemCommandsGetLocation, "1\n", 3);

        CMD_API_PuttoQueue(eCommandModulesModem, eModemCommandSendLocation, "1\n", 3 );
        osDelay(10000);
        power_save_mode_counter = 0;
        Pet_Tracker_Stop();
    }



}



void Pet_Tracker_ModemStart(){
    UART_API_SendString(eUartDebug, "Modem on\n", 20);
    CMD_API_PuttoQueue(eCommandModulesPower, (uint8_t)ePowerCommandsModemPower, "1\n", 3);

    osDelay(20000);
    Blinky();

    CMD_API_PuttoQueue(eCommandModulesModem, (uint8_t)eModemCommandsSendCommand, "AT+CMGF=1\n", 50);
    osDelay(2000);

    Blinky();

    CMD_API_PuttoQueue(eCommandModulesModem, (uint8_t)eModemCommandsGNSSPower, "1\n", 3);
    osDelay(15000);

    Blinky();

    CMD_API_PuttoQueue(eCommandModulesModem, (uint8_t)eModemCommandsSendSMS, "+37067852939, Tracker started !\n", 50);

    pet_tracker_status |= TRACKER_ON;
}

void Pet_Tracker_Start() {
    //UART_API_SendString(eUartDebug, "AcceInit\n", 20);
    //ACC_API_WriteRegister(eAccMPU6050, PWR_MGMT1_REGISTER, 0x00);
    //acce_status |= ACCE_ENABLE;
    //ACC_API_Resume();
    //osDelay(6000);
    //acce_status &= ~ACCE_ENABLE;
    //osDelay(3000);
    //UART_API_SendString(eUartDebug, "4V on\n", 20);
    //CMD_API_PuttoQueue(eCommandModulesPower, (uint8_t)ePowerCommands4VPower, "1\n", 3);
    //osDelay(5000);
    Pet_Tracker_Read_Options();


}

void Pet_Tracker_Stop(){
    ACC_API_WriteRegister(eAccMPU6050, PWR_MGMT1_REGISTER, 0x80); //stop acce

    CMD_API_PuttoQueue(eCommandModulesPower, (uint8_t)ePowerCommandsModemPower, "0\n", 3);
    pet_tracker_status &= ~GNSS_POWER_ON;
    pet_tracker_status &= ~TRACKER_ON;

}













void SystemClock_Config(void){
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
}

void Error_Handler(void){
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line){
}
#endif /* USE_FULL_ASSERT */


