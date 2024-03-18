/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/

//#include "debug_api.h"
#include "uart_driver.h"
#include "uart_api.h"
#include "cmd_api.h"
//#include "accelerometer_api.h"


#define UART_QUEUE_PUT_TIMEOUT 100
#define UART_QUEUE_SIZE 20
#define DEBUG_UART_GOT_MESSAGE 0x01
#define MODEM_UART_GOT_MESSAGE 0x02
#define DEBUG_UART_MESSAGE_PUT_IN_QUEUE 0x04
#define MODEM_UART_MESSAGE_PUT_IN_QUEUE 0x08
 
#define END_SYMBOL '\n'
#define END_SYMBOL2 '>'


uint8_t uart_flag = 0;
//RTOS VARIABLES 
const osThreadAttr_t uart_api_thread_atr = {
    .name = "uart_api_thread",
    .stack_size = 1024,
    .priority = osPriorityHigh
};
const osMessageQueueAttr_t uart_data_queue_atr_debug = {.name = "uart_data_queue"};
osMessageQId uart_data_queue_id;
const osMutexAttr_t debug_send_string_mutex_atr    = {.name = "debug_send_string_mutex"};
const osMutexAttr_t modem_send_string_mutex_atr    = {.name = "modem_send_string_mutex"};
osEventFlagsId_t uart_flags = NULL;
osThreadId_t uart_thread_id = NULL;
sUartData_t message = {0};


//UART DATA STRUCTURES. 
static sUartControl_t uart_buffer_lut[] = {
    [eUartDebug] = {
        .buffer = NULL, 
        .index = 0, 
        .state = eUartStateLast, 
        .size = 1024, 
        .mutex_id = NULL,
        .mutex_atr = &debug_send_string_mutex_atr, 
        .uart_flag = DEBUG_UART_GOT_MESSAGE
    },
    [eUartModem] = {
        .buffer = NULL, 
        .index = 0, 
        .state = eUartStateLast, 
        .size = 1024, 
        .mutex_id = NULL,
        .mutex_atr = &modem_send_string_mutex_atr, 
        .uart_flag = MODEM_UART_GOT_MESSAGE
    },
};

//FUNCTIONS 
uint32_t UART_API_WaitFlag ();


bool UART_API_Init (eUart_t uart_id, eBaudRate_t baudrate) {
    
    //INITIALIZE UART FLAGS TO KNOW WHEN EXTRACT MESSAGES

    if (uart_flags == NULL) {
        uart_flags = osEventFlagsNew(NULL);
        if (uart_flags == NULL) {
            return false;
        }
    }

    //INITIALIZE UART DRIVER AND GIVE SETFLAG FUNCTION 
    if (UART_Driver_Init(uart_id, baudrate, &UART_API_SetFlag) != true) {
        return false;
    }


    //CREATE MUTEX'ES FOR THREADS TO USE 
    uart_buffer_lut[uart_id].mutex_id = osMutexNew(uart_buffer_lut[uart_id].mutex_atr);
    if (uart_buffer_lut[uart_id].mutex_id == NULL) {
        return false;
    }

    //CREATE MESSAGE QUEES TO PUT MESSAGES FOR CMD API 
    if (uart_data_queue_id == NULL){ 
        uart_data_queue_id = osMessageQueueNew(UART_QUEUE_SIZE, sizeof(sUartData_t), &uart_data_queue_atr_debug);
        if (uart_data_queue_id == NULL) {
            return false;
        }
    }
    

    //INITIALIZE CMD THREAD FOR CUTTING UART DATA INTO COMMANDS 

    if (CMD_API_ThreadInit() == false){ 
        return false; 
    }


    //INITIALIZE STATE MACHINE AS INIT 
    uart_buffer_lut[uart_id].state = eUartStateInit;
    if (uart_thread_id == NULL) {
        uart_thread_id = osThreadNew(UART_API_Thread, NULL, &uart_api_thread_atr);
        if (uart_thread_id == NULL) {
            //error("Cannot Innitialize UART_API_Thread");
            return false;
        }
    }
    


   // if (uart_interupt_id == NULL) {
     //   uart_interupt_id = osThreadNew(UART_Interupt_Thread, NULL, &uart_interupt_thread_atr);
    //    if (uart_interupt_id == NULL){
      //      return false;
   //     }
   // }
    return true;
}

void UART_API_Thread (void *argument) {
    uint32_t uart_got_message = 0;
	while (true) {
		uart_got_message = 0;
        uart_got_message = UART_API_WaitFlag();
        if (uart_got_message != 0){
            eUart_t uart; 
            if ((uart_got_message & DEBUG_UART_GOT_MESSAGE) != 0) uart = eUartDebug; 
            else if ((uart_got_message & MODEM_UART_GOT_MESSAGE) != 0) uart = eUartModem; 
            switch (uart_buffer_lut[uart].state) {
                case eUartStateInit: {
                    uart_buffer_lut[uart].buffer = (uint8_t*) calloc(uart_buffer_lut[uart].size, sizeof(uint8_t));
                    uart_buffer_lut[uart].index = 0;
                    uart_buffer_lut[uart].state = eUartStateRead;
                }
                case eUartStateRead: {
                    uint8_t byte = 0;
                    while (UART_Driver_GetByte(uart, &byte) == true) {
                        if (byte == END_SYMBOL || byte == END_SYMBOL2) {
                            uart_buffer_lut[uart].buffer[uart_buffer_lut[uart].index] = byte;
                            uart_buffer_lut[uart].index++;
                            uart_buffer_lut[uart].state = eUartStateSend;
                            break;
                        } else {
                            uart_buffer_lut[uart].buffer[uart_buffer_lut[uart].index] = byte;
                            uart_buffer_lut[uart].index++;
                        }
                    }
                    if (uart_buffer_lut[uart].state != eUartStateSend) {
                        break;
                    }
                }
                case eUartStateSend:
                    message.buffer_adress = (char*)uart_buffer_lut[uart].buffer;
                    message.size = uart_buffer_lut[uart].index;
                    if (osMessageQueuePut(uart_data_queue_id, &message, osPriorityHigh, UART_QUEUE_PUT_TIMEOUT) != osOK) {

                    }
                    uart_buffer_lut[uart].state = eUartStateInit;
                    break;
                default:
                    break;
            }
        }
    }
    osThreadTerminate(uart_thread_id);
}







bool UART_API_SendString (eUart_t uart, char *string, uint16_t size) {
    if (osMutexAcquire(uart_buffer_lut[uart].mutex_id, osWaitForever) != osOK) {
        return false;
    }
    if (UART_Driver_Send_String((uint8_t*) string, size, uart) != true) {
        osMutexRelease(uart_buffer_lut[uart].mutex_id);
        return false;
    }
    osMutexRelease(uart_buffer_lut[uart].mutex_id);
    return true;
}


bool UART_API_GetMessage (sUartData_t *acquired_buffer, eUart_t uart, uint32_t wait_time) {
    if (osMessageQueueGet(uart_data_queue_id, acquired_buffer, NULL, wait_time) != osOK) {
        return false;
    }
    return true;
}

uint32_t UART_API_WaitFlag () {
    uint32_t flag = osEventFlagsWait(uart_flags, DEBUG_UART_GOT_MESSAGE | MODEM_UART_GOT_MESSAGE, osFlagsWaitAny, osWaitForever);
    return flag;
}

bool UART_API_SetFlag (eUart_t uart) {
    
    if (uart == eUartDebug){
    	if (osEventFlagsSet(uart_flags, DEBUG_UART_GOT_MESSAGE) == 0) return false;
    }
    else if (uart == eUartModem)
    	if (osEventFlagsSet(uart_flags, MODEM_UART_GOT_MESSAGE) == 0) return false;

    return true;
}




/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
