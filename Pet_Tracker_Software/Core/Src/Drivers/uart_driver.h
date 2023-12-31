#ifndef __UART_DRIVER__H__
#define __UART_DRIVER__H__
/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "stdbool.h"
#include "main.h"
#include "gpio_driver.h"
//#include "ring_bufer.h"

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
typedef enum eUart_t {
    eUartFirst = 0,
    eUartDebug = eUartFirst,
    eUartModem,
    eUartLast
} eUart_t;


typedef enum eBaudRate_t {
    eBaudRateFirst = 0,
    eBaudRate1200 = eBaudRateFirst,
    eBaudRate2400,
    eBaudRate4800,
	eBaudRate9600,
    eBaudRate19200,
    eBaudRate38400,
    eBaudRate57600,
    eBaudRate115200,
    eBaudRateLast
}eBaudRate_t;


bool UART_Driver_Init (eUart_t uart, eBaudRate_t baudrate);
bool UART_Driver_Low_Level_Init(eUart_t); 
// bool UART_Driver_TansmitByte (eUart_t uart, uint8_t symbol);
// bool UART_Driver_EnableInterupt (eUart_t uart);
bool UART_Driver_Send_String (); //
// bool UART_Driver_GetByte(eUart_t uart, uint8_t *byte);
// bool Modem_API_SendSms (char *buffer, uint16_t size);
#endif /* __UART_DRIVER__H__ */
