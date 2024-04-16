#include "modem_api.h"
#include "uart_api.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "cmsis_os2.h"
#include "cmd_api.h"
#include "stdarg.h"


#define MODEM_WAIT_FOR_SMS 	(1U<<0)
#define MODEM_OK 			(1U<<1)
#define MODEM_ERROR			(1U<<2)

typedef enum eModemATCommandAnswer { 
    eModemATFirst = 0,
    eModemATOK = eModemATFirst, 
    eModemATError, 
    eModemATCMGI, 
    eModemATCMGF,
	eModemATCPIN,
    eModemATCMGS,
    eModemATWait, 
    eModemATLast, 
} eModemATCommandAnswer;

const char modem_commands_lut[eModemATLast][20] = { 
    [eModemATOK]        = "OK",
    [eModemATError]     = "ERROR",
    [eModemATCMGI]      = "+CMGI:",
    [eModemATCMGF]      = "+CMGF:",
	[eModemATCPIN]		= "+CPIN:", 
    [eModemATCMGS]      = "+CMGS:",
    [eModemATWait]      = ">"
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

osMutexId_t modem_mutex_id = NULL; 
const osMutexAttr_t modem_mutex_atr    = {.name = "modem_send_mutex"};


//message queue for commands
const osMessageQueueAttr_t modem_answer_queue_atr = {.name = "modem_answers_queue"};
osMessageQId modem_answer_queue_id;
osEventFlagsId_t modem_flags = NULL;



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

    if (modem_mutex_id == NULL){
		modem_mutex_id = osMutexNew(&modem_mutex_atr);
		if (modem_mutex_id == NULL) {
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

    if (modem_flags == NULL) {
    	modem_flags = osEventFlagsNew(NULL);
    	if (modem_flags == NULL) {
    		return false;
    	}
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
                    osEventFlagsSet(modem_flags, MODEM_OK); 
                    break; 
                } 
                case eModemATError:{
                    osEventFlagsSet(modem_flags, MODEM_ERROR); 
                    break;
                } 
                case eModemATCMGI:{
                    break;
                } 
                case eModemATCMGF:{
                    break;
                }
	            case eModemATCPIN:{
	            	
                    break;
                }
                case eModemATWait:{ 
                    osEventFlagsSet(modem_flags, MODEM_WAIT_FOR_SMS);
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

    Modem_API_SendWait(params, eModemSendWaitNo);
    return true;
}


bool Modem_API_SendSMS(char *params){ 
    char temp_buf[100]={0}; 
    char modem_tx[100] = {0};
    //PARSE PHONE NUMBER
    uint8_t i = 0;
    uint8_t t_index = 0;
    uint8_t flag = 0;
    for (i = 0; i<10; i++){
        if (params[i] == '+'){
            temp_buf[t_index++] = params[i];
            break; //find '+' sign 
        }
    }
    i++;
    if (i == 10) return false;   
    for ( ; i <30; i++){ // parse phone 
        if (params[i]>='0' && params[i]<='9') temp_buf[t_index++] = params[i];
        if (params[i] == ',') break; //
    }

    if (t_index > 5){ //assuming that phone number is good 
        snprintf(modem_tx,120,"AT+CMGS=\"%s\"\n", temp_buf);
        if (Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_WAIT_FOR_SMS))
        	flag = 1;
        else flag = 0;
    }  

    for ( ; t_index>0; t_index--){ 
        temp_buf[t_index] = 0 ; 
    }
    temp_buf[0] = 0 ;
    i++; 

    for ( ; i<130; i++){ 
        if ((params[i] != '\n') || (params[i] != 0))
            {
                temp_buf[t_index++] = params[i]; 
            }
        if (params[i] == '\n') break;
    }
    temp_buf[t_index++] = 26;
    if (flag == 1){
    	if(Modem_API_SendWait(temp_buf, eModemSendWaitFlagCustom, 2000, MODEM_OK))
    		flag = 2;
    }
    




    return true;
}


bool Modem_API_SendWait(char * params, eModemSendWait modem_wait, ...){
    uint32_t wait_time =  0xFFFFFFFF; 
    uint32_t wait_flag =  0xFFFFFFFF;
    uint32_t got_flag;
    va_list arg_list; 
    va_start (arg_list, modem_wait);
    if ((modem_wait == eModemSendWaitCustom) 
        ||(modem_wait == eModemSendWaitFlagCustom)){
            wait_time = va_arg(arg_list, uint32_t); 
    } else {
    	wait_time = 0;
    }
    if ((modem_wait == eModemSendWaitFlagCustom) 
        ||(modem_wait == eModemSendWaitFlagForever)){
    		wait_flag = va_arg(arg_list, uint32_t);
    		if (wait_time == 0) wait_time = 0xFFFFFFFF;
    }
    va_end(arg_list);
    if (osMutexAcquire(modem_mutex_id, 0) != osOK){
        return false; //mutex is not free
    } else {
        if ((modem_wait == eModemSendWaitFlagCustom) 
            ||(modem_wait == eModemSendWaitFlagForever)){
                osEventFlagsClear(modem_flags, wait_flag);
        }
        UART_API_SendString(eUartModem, params, 250);
        switch (modem_wait) {
        	case eModemSendWaitNo: {
        		osMutexRelease(modem_mutex_id);
        		return true;
        	}
        	case eModemSendWaitCustom: {
        		osDelay(wait_time);
        		osMutexRelease(modem_mutex_id);
        		return true;
        	}
        	case eModemSendWaitFlagCustom:
        	case eModemSendWaitFlagForever:{
				got_flag = osEventFlagsWait(modem_flags, wait_flag, osFlagsWaitAll, wait_time);
        		osMutexRelease(modem_mutex_id);
        		if ((got_flag & wait_flag) == wait_flag){
        			return true;
        		} else {
        			return false;
        		}
        	}
        	default: {
        		osMutexRelease(modem_mutex_id);
        		return false;
        	}
        }
    }
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



















