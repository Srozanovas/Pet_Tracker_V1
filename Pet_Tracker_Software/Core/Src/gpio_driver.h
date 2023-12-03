#ifndef __GPIO_DRIVER__H__
#define __GPIO_DRIVER__H__
/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/

#include "stdbool.h"
#include "stdarg.h"

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
typedef enum eGpioPin_t {
	eGpioPinFirst = 0,
	//GPIOA----------------
	eGpioPinA1,
	eGpioPinA2UART2TX, 		//USART2TX
	eGpioPinA3UART2RX, 		//USART2RX 
	eGpioPinA4AcceInt, 		//accelerometer interupt 
	eGpioPinA5GSMEnable,	//turn on
	eGpioPinA6,
	eGpioPinA7,
	eGpioPinA9I2C1SCL,		//I2C1 SCl
	eGpioPinA10I2C1SDA, 	//I2C1 SDA
	eGpioPinA11,
	eGpioPinA12,
	eGpioPinA15,
	//--------------------
	//GPIOB---------------
	eGpioPinB0,
	eGpioPinB1,
	eGpioPinB2,
	eGpioPinB6UART1TX,		//UART1TX 
	eGpioPinB7UART1RX,		//UART1RX
	eGpioPinB10I2C2SCL, 	//I2C2 SCL 
	eGpioPinB11I2C2SDA, 	//I2C2 SDA
	eGpioPinB12,
	eGpioPinB13,
	eGpioPinB14,
	eGpioPinB15,
	//--------------------
	//GPIOC---------------
	eGpioPinC4,
	eGpioPinC6,
	eGpioPinC7,
	eGpioPinC8,
	eGpioPinC9,
	//--------------------

	eGpioPinLast
}eGpioPin_t;


typedef enum eGpioPinState_t {
	ePinHigh,
	ePinLow,
	ePinNaN, 
	ePinLast
} eGpioPinState_t;

typedef enum eGpioAllPin_t {
	eGpioAllPinsOK,
    eGpioAllPinsNotOk
} eGpioAllPin_t;




/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
bool GPIO_Driver_Init (eGpioPin_t s_gpio_pin, ...);
bool GPIO_Driver_TogglePin (eGpioPin_t s_gpio_pin);
bool GPIO_Driver_WritePin (eGpioPin_t s_gpio_pin, 	eGpioPinState_t pin_state);
bool GPIO_Driver_ReadPin (eGpioPin_t s_gpio_pin, 	eGpioPinState_t *pin_status);
bool GPIO_Driver_InitAll (void);



#endif /* __GPIO_DRIVER__H__ */
