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
	eGpioDriverPinFirst = 0,
	eGpioDriverLedStatPin = eGpioDriverPinFirst,
	eGpioDriverUartDebugTX,
	eGpioDriverUartDebugRX,
	eGpioDriverUartModemTX,
	eGpioDriverUartModemRX,
	eGpioDriverModemEnable, 
	eGpioDriverI2CEepromSCL,
	eGpioDriverI2CEepromSDA,
	eGpioDriverI2CAcceSCL,
	eGpioDriverI2CAcceSDA,
	eGpioDriverAcceInterupt,
	eGpioDriverPA12,
	eGpioDriverPB0,
	eGpioDriverPB1,
	eGpioDriverPB2,
	eGpioDriverPinLast
}eGpioPin_t;

typedef enum eGpioPinState_t {
	ePinOn,
	ePinOff,
	ePinLast
} eGpioPinState_t;

typedef enum eGpioPinStatus_t {
	ePinHigh,
	ePinLow,
	ePinNaN
} eGpioPinStatus_t;

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
bool GPIO_Driver_WritePin (eGpioPin_t s_gpio_pin, eGpioPinState_t pin_state);
bool GPIO_Driver_ReadPin (eGpioPin_t s_gpio_pin, eGpioPinStatus_t *pin_status);
bool GPIO_Driver_InitAll (void);



#endif /* __GPIO_DRIVER__H__ */
