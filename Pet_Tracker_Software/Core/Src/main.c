#include "main.h"
#include "cmsis_os.h"
#include "gpio_driver.h"
#include "uart_api.h"
#include "cli_app.h"
#include "cmd_api.h"
#include "modem_api.h"


uint32_t gpio_init_status = 0;
uint32_t gpio_pin_level_status = 0;
uint16_t  uart_init_status = 0;
uint16_t pet_tracker_init_status = 0;
uint32_t pet_tracker_status = 0;
uint16_t threads_status = 0 ;







osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void StartDefaultTask(void *argument);
bool Main_APP_Pet_Tracker_Init(void); 






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
    GPIO_Driver_Init(eGpioPinB0Power4V, ePinLow);
	GPIO_Driver_Init(eGpioPinA6GSMPower, ePinHigh);
    GPIO_Driver_Init(eGpioPinA12LEDsOn, ePinLow);
    UART_API_Init(eUartModem, eBaudRate9600);
    UART_API_Init(eUartDebug, eBaudRate19200);
    CLI_APP_Init();
    CMD_API_ThreadInit();
    Modem_API_Init();
    return true;

}




void StartDefaultTask(void *argument)
{
 
  for(;;)
  {
    osDelay(1);
  }
 
}













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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}


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


void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {}
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
