#include "main.h"
#include "cmsis_os.h"


#include "gpio_driver.h"
#include "uart_driver.h"
#include "ring_bufer.h"
#include "i2c_driver.h"
#include "uart_api.h"
#include "cli_app.h"


osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};




uint32_t pt_status = 0x00;






void SystemClock_Config(void);
void StartDefaultTask(void *argument);
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  osKernelInitialize();
  CLI_APP_Init();
  if (UART_API_Init(eUartDebug, eBaudRate19200)) pt_status |= DEBUG_UART_ENABLED;
  else (pt_status &= ~DEBUG_UART_ENABLED);
  if (UART_API_Init(eUartModem, eBaudRate9600)) pt_status |= MODEM_UART_ENABLED;
  else (pt_status &= ~MODEM_UART_ENABLED);
  GPIO_Driver_Init(eGpioPinA12LEDsOn, ePinLow);
  GPIO_Driver_Init(eGpioPinA6GSMPower, ePinHigh);
  GPIO_Driver_Init(eGpioPinB0Power4V, ePinHigh);


  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  osKernelStart();
  



  while (1){

  
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


void StartDefaultTask(void *argument)
{
	for(;;){
    osDelay(500);
  }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
 
}

void Error_Handler(void)
{
  __disable_irq();
  while (1){}

}
