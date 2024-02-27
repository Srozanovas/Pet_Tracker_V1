
#include "main.h"
#include "cmsis_os.h"


#include "Drivers/gpio_driver.h"
#include "Drivers/uart_driver.h"
#include "Utility/ring_bufer.h"
#include "Drivers/i2c_driver.h"




osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};


void SystemClock_Config(void);
void StartDefaultTask(void *argument);
int main(void)
{
  HAL_Init();
  SystemClock_Config();





  GPIO_Driver_Init(eGpioPinA12LEDsOn); 
  GPIO_Driver_Init(eGpioPinA6GSMPower); 
  GPIO_Driver_WritePin(eGpioPinA6GSMPower, ePinHigh); 
  GPIO_Driver_Init(eGpioPinB0Power4V);
  GPIO_Driver_WritePin(eGpioPinB0Power4V, ePinHigh); 
  HAL_Delay(50);
  GPIO_Driver_TogglePin(eGpioPinA6GSMPower); 
  HAL_Delay(1000); 
  GPIO_Driver_TogglePin(eGpioPinA6GSMPower); 


  //osKernelInitialize();
  //defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  //osKernelStart();
  
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
