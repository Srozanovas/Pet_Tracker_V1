#include "modem_api.h"
#include "uart_api.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "cmsis_os2.h"
#include "cmd_api.h"




typedef enum eModemATCommandAnswer { 
    eModemATFirst = 0,
    eModemATOK = eModemATFirst, 
    eModemATError, 
    eModemATCMGI, 
    eModemATCMGF,
	eModemATCPIN,
    eModemATLast, 
} eModemATCommandAnswer;

const char modem_commands_lut[eModemATLast][20] = { 
    [eModemATOK]        = "OK",
    [eModemATError]     = "ERROR",
    [eModemATCMGI]      = "+CMGI:",
    [eModemATCMGF]      = "+CMGF:",
	[eModemATCPIN]		= "+CPIN:"
}; 






typedef struct sModemMessage { 
    eModemATCommandAnswer at_command; 
    char *command_answer; 
}sModemMessage; 



const osThreadAttr_t modem_api_thread_atr = {
    .name = "modem_api_thread",
    .stack_size = 512,
    .priority = osPriorityHigh
};
osThreadId_t modem_api_thread_id = NULL;

const osThreadAttr_t modem_answers_thread_atr = {
    .name = "modem_answers_thread",
    .stack_size = 512,
    .priority = osPriorityNormal
};
osThreadId_t modem_answers_thread_id = NULL;



//message queue for commands
const osMessageQueueAttr_t modem_answer_queue_atr = {.name = "modem_answers_queue"};
osMessageQId modem_answer_queue_id;



//DATA VARIABLES
sUartData_t modem_uart_data_unproccessed = {0}; //data from uart



void Modem_API_MessageThread ();
void Modem_API_AnswerThread();

bool Modem_API_Init(){ 
    if (modem_answer_queue_id == NULL) {
        modem_answer_queue_id = osMessageQueueNew(COMMAND_QUEUE_SIZE, sizeof(sModemMessage *), &modem_answer_queue_atr);
        if (modem_answer_queue_id == NULL) {
        	return false;
        } 
    }

    if (modem_api_thread_id == NULL){
		modem_api_thread_id = osThreadNew(Modem_API_MessageThread, NULL, &modem_api_thread_atr);
		if (modem_api_thread_id == NULL) return false;
    }

    if (modem_answers_thread_id == NULL){
    	modem_answers_thread_id = osThreadNew(Modem_API_AnswerThread, NULL, &modem_answers_thread_atr);
   		if (modem_answers_thread_id == NULL) return false;
       }


    return true;  
}

bool Modem_API_MessageParser(sUartData_t * raw_data, sModemMessage *parsed_message) { 
    char temporary[20] = {0};
    uint8_t index = 0; 
    uint8_t i = 0;
    parsed_message->at_command = eModemATLast;
    for (i = 0;  i < 20; i++){ 
        if (raw_data->buffer_adress[i] == '\n') break;
        else if ((raw_data->buffer_adress[i] != '\n') && (raw_data->buffer_adress[i] != 13)) { 
            temporary[index++] = raw_data->buffer_adress[i];
            if (temporary[index-1] == ':') break;
        }
    }
    if (index == 0) return true; //PROBABLY CR CF MESSAGE 

    for(eModemATCommandAnswer i = 0; i < eModemATLast; i ++){ 
        if (strncmp(modem_commands_lut[i], temporary, index) == 0) {
            parsed_message->at_command = (eModemATCommandAnswer)(i);
            break; 
        }
    }
    if (parsed_message->at_command == eModemATLast) return false;  //no command answer found on LUT 
    i++;
    index = 0; 
    for (  ; i<raw_data->size; i++){ 
        parsed_message->command_answer[index++] = raw_data->buffer_adress[i];
        if (parsed_message->command_answer[index-1] == '\n'){
            break;
        }
    }
    return true; //everythings allright
}



void Modem_API_MessageThread ()  { 
    while(1) {
        if (UART_API_GetModemMessage(&modem_uart_data_unproccessed, osWaitForever) == true) {
            if (1)
            UART_API_SendString(eUartDebug, modem_uart_data_unproccessed.buffer_adress, modem_uart_data_unproccessed.size);
            
            sModemMessage *modem_message = calloc(1, sizeof(sModemMessage));
            modem_message->command_answer = calloc(100, sizeof(char));

            Modem_API_MessageParser(&modem_uart_data_unproccessed, modem_message);
            free (modem_uart_data_unproccessed.buffer_adress);
            if (modem_message->at_command == eModemATLast){
            	free(modem_message->command_answer);
            	free(modem_message);
            }else {
				osMessageQueuePut(modem_answer_queue_id, &modem_message, osPriorityHigh, COMMANDS_QUEUE_PUT_TIMEOUT);
            }
        }
        
    }
    osThreadTerminate(modem_api_thread_id); 
}


void Modem_API_AnswerThread(){
	while(1){
        sModemMessage* parsed_message = NULL; 
        osStatus_t status; 
        status = osMessageQueueGet(modem_answer_queue_id, &parsed_message, NULL , osWaitForever);
        if (status != osOK){
            if (parsed_message != NULL){
                free(parsed_message->command_answer); 
                free(parsed_message);
            }
        } 
        else { 
            switch(parsed_message->at_command) {
                case eModemATOK: { 
                    break; 
                } 
                case eModemATError:{
                    break;
                } 
                case eModemATCMGI:{
                    break;
                } 
                case eModemATCMGF:{
                    break;
                }
	            case eModemATCPIN:{
	            	__NOP();
                    break;
                }
                case eModemATLast:{
                    break;
                } 
                default: break;  
            }
            free(parsed_message->command_answer); 
            free(parsed_message);
        }
	}
	osThreadTerminate(modem_answers_thread_id);
}


bool Modem_API_SendCommand(char * params){
    UART_API_SendString(eUartModem, params, 250);
    return true;
}

bool Modem_API_GNSS_Power(char *params){
    uint8_t i=0;
    while (1) { 
        if (params[i]==0 || params[i]=='\n') return false; //no data
        if (params[i]-48 == 1 || params[i]-48 == 0) i = params[i] - 48;  //extract 1 or 0 
    }
    char modem_tx[20]={0};
    strncpy(modem_tx, "AT+CGNSPWR=%d\n", i);
    return true;
}






bool Modem_API_ATOK(char *params){
	return true;
}
















