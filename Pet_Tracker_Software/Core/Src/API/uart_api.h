#ifndef __USART_API__H__
#define __USART_API__H__


#include "cmsis_os.h"
#include "uart_driver.h"
//#include "cli_function_list.h"


typedef enum eUartStateStates_t {
        eUartStateFirst = 0,
        eUartStateInit = eUartStateFirst,
        eUartStateIdle,
        eUartStateRead,
        eUartStateSend,
        eUartStateLast
} eUartStringStates_t;

typedef struct sUartControl_t {
    uint32_t index;
    uint8_t *buffer;
    uint16_t size;
    eUartStringStates_t state;
    const osMutexAttr_t* mutex_atr;
    osMutexId_t mutex_id;
    uint32_t uart_flag;
} sUartControl_t;

typedef struct sUartData_t {
    char *buffer_adress;
    uint16_t size;
} sUartData_t;





/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
bool UART_API_Init (eUart_t uart_id, eBaudRate_t baudrate);
void UART_API_Thread (void *argument);
bool UART_API_SendString (eUart_t uart,char *string, uint16_t size );
bool UART_API_GetMessage (sUartData_t *acquired_buffer, uint32_t wait_time);
bool UART_API_SetFlag(eUart_t uart);


#endif /* __SOMETHING__H__ */
