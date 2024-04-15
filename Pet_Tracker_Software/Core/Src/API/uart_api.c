#include "uart_driver.h"
#include "uart_api.h"
#include "cmd_api.h"


#define UART_QUEUE_PUT_TIMEOUT 100 
#define UART_QUEUE_SIZE 20

//os flags
#define DEBUG_UART_GOT_MESSAGE 0x01
#define MODEM_UART_GOT_MESSAGE 0x02
#define DEBUG_UART_MESSAGE_PUT_IN_QUEUE 0x04
#define MODEM_UART_MESSAGE_PUT_IN_QUEUE 0x08
 
#define END_SYMBOL '\n'
#define END_SYMBOL2 '>'


uint8_t uart_flag = 0;
//RTOS VARIABLES
//uart api thread  
const osThreadAttr_t uart_api_thread_atr = {
    .name = "uart_api_thread",
    .stack_size = 2048,
    .priority = osPriorityHigh
};
osThreadId_t uart_thread_id = NULL;

//uart data queue
const osMessageQueueAttr_t command_data_queue_atr= {.name = "command_data_queue"};
osMessageQId command_data_queue_id;
const osMessageQueueAttr_t modem_data_queue_atr= {.name = "modem_data_queue"};
osMessageQId modem_data_queue_id;

//uart mutexes
const osMutexAttr_t debug_send_string_mutex_atr    = {.name = "debug_send_string_mutex"};
const osMutexAttr_t modem_send_string_mutex_atr    = {.name = "modem_send_string_mutex"};

//uart flags
osEventFlagsId_t uart_flags = NULL;


//not parsed raw uart data
sUartData_t uart_data = {0};


//UART DATA STRUCTURES. 
static sUartControl_t uart_buffer_lut[] = {
    [eUartDebug] = {
        .buffer = NULL, 
        .index = 0, 
        .state = eUartStateLast, 
        .size = 250,
        .mutex_id = NULL,
        .mutex_atr = &debug_send_string_mutex_atr, 
        .uart_flag = DEBUG_UART_GOT_MESSAGE
    },
    [eUartModem] = {
        .buffer = NULL, 
        .index = 0, 
        .state = eUartStateLast, 
        .size = 250,
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
        } else {
            uart_init_status |= UART_FLAGS_INIT; 
        }
    }

    //INITIALIZE UART DRIVER AND GIVE SETFLAG FUNCTION 
    if (UART_Driver_Init(uart_id, baudrate, &UART_API_SetFlag) != true) {
    	return false;
    } else { 
        uart_init_status |=  (uart_id == eUartDebug)? DEBUG_UART_DRIVER_INIT : MODEM_UART_DRIVER_INIT; 
    }


    //CREATE MUTEX'ES FOR THREADS TO USE 
    if (uart_buffer_lut[uart_id].mutex_id == NULL){
		uart_buffer_lut[uart_id].mutex_id = osMutexNew(uart_buffer_lut[uart_id].mutex_atr);
		if (uart_buffer_lut[uart_id].mutex_id == NULL) {
			return false;
		} else { 
            uart_init_status |= (uart_id == eUartDebug)? DEBUG_UART_MUTEX_INIT : MODEM_UART_MUTEX_INIT;
        }
    }

    //CREATE MESSAGE QUEES TO PUT MESSAGES FOR CMD API 
    if (command_data_queue_id == NULL){ 
        command_data_queue_id = osMessageQueueNew(UART_QUEUE_SIZE, sizeof(sUartData_t), &command_data_queue_atr);
        if (command_data_queue_id == NULL) {
        	return false;
        } else{ 
            uart_init_status |= DEBUG_MESSAGE_QUEUE_INIT; 
        }
    }
    if (modem_data_queue_id == NULL){ 
        modem_data_queue_id = osMessageQueueNew(UART_QUEUE_SIZE, sizeof(sUartData_t), &modem_data_queue_atr);
        if (modem_data_queue_id == NULL) {
            return false;
        } else{ 
            uart_init_status |= MODEM_MESSAGE_QUEUE_INIT; 
        }
    }


    

    //INITIALIZE CMD THREAD FOR CUTTING UART DATA INTO COMMANDS 

    if (CMD_API_ThreadInit() == false){ 
    	return false;
    } else { 
        pet_tracker_init_status |= CMD_API_INITIALISED;
    }


    //INITIALIZE STATE MACHINE AS INIT 
    uart_buffer_lut[uart_id].state = eUartStateInit;
    if (uart_thread_id == NULL) {
        uart_thread_id = osThreadNew(UART_API_Thread, NULL, &uart_api_thread_atr);
        if (uart_thread_id == NULL) {
            return false;
        } else { 
            threads_status |= UART_API_THREAD_INIT;
        }
    }
    
    uart_init_status |= (uart_id == eUartDebug)? DEBUG_UART_ENABLED : MODEM_UART_ENABLED;
    return true;
}



void UART_API_Thread (void *argument) {
    uint32_t uart_got_message = 0;
	while (true) {
		uart_got_message = 0;
        uart_got_message = UART_API_WaitFlag();
        if (uart_got_message != 0){
            eUart_t uart = eUartDebug;
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
                	uart_data.buffer_adress = (char*)uart_buffer_lut[uart].buffer;
                	uart_data.size = uart_buffer_lut[uart].index;
                	if (uart == eUartModem){
                		if (osMessageQueuePut(modem_data_queue_id, &uart_data, osPriorityHigh, UART_QUEUE_PUT_TIMEOUT) != osOK) {}
                	} else {
                        if (osMessageQueuePut(command_data_queue_id, &uart_data, osPriorityHigh, UART_QUEUE_PUT_TIMEOUT) != osOK) {}
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
    if (UART_Driver_Send_String( uart, (uint8_t*) string, size) != true) {
        osMutexRelease(uart_buffer_lut[uart].mutex_id);
        return false;
    }
    osMutexRelease(uart_buffer_lut[uart].mutex_id);
    return true;
}


bool UART_API_GetMessage (sUartData_t *acquired_buffer, uint32_t wait_time) {
	osStatus_t stat;
    stat = osMessageQueueGet(command_data_queue_id, acquired_buffer, NULL, wait_time);
	if ( stat != osOK) {
        return false;
    }
    return true;
}


bool UART_API_GetModemMessage(sUartData_t *acquired_buffer, uint32_t wait_time){ 
    osStatus_t stat;
    stat = osMessageQueueGet(modem_data_queue_id, acquired_buffer, NULL, wait_time);
	if ( stat != osOK) {
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
    else if (uart == eUartModem){
        if (osEventFlagsSet(uart_flags, MODEM_UART_GOT_MESSAGE) == 0) return false;
    }
    return true;
}




/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
