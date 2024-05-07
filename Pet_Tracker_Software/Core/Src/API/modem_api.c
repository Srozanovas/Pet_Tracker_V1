#include "modem_api.h"
#include "uart_api.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "cmsis_os2.h"
#include "cmd_api.h"
#include "stdarg.h"
#include "cli_function_list.h"
#include "eeprom_api.h"


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
    eModemATCGNSCHK,
    eModemATCGNSINF, 
    eModemATCMGR, 
    eModemATCMTI,
	eModemATHTTPACTION,
	eModemATPSUTTZ,
	eModemATDST,
	eModemATNotParsed,
    eModemATCCLK,
    eModemATLast, 
} eModemATCommandAnswer;
const char modem_commands_lut[eModemATLast][20] = { 
    [eModemATOK]        = "OK",
    [eModemATError]     = "ERROR",
    [eModemATCMGI]      = "+CMGI:",
    [eModemATCMGF]      = "+CMGF:",
	[eModemATCPIN]		= "+CPIN:", 
    [eModemATCMGS]      = "+CMGS:",
    [eModemATWait]      = ">",
    [eModemATCGNSCHK]   = "+CGNSCHK:",
    [eModemATCGNSINF]   = "+CGNSINF:",
    [eModemATCMGR]      = "+CMGR:", 
    [eModemATCMTI]      = "+CMTI:",
    [eModemATHTTPACTION]= "+HTTPACTION:",
	[eModemATPSUTTZ]	= "*PSUTTZ:",
	[eModemATDST]		= "DST:",
    [eModemATCCLK]      = "+CCLK:"
}; 




//RTOS VARIABLES ----------------------

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
const char crcf[4] = {0x0D, 0x0A, 0x00, 0x00};

//functions 
//THREADS
void Modem_API_MessageThread ();
void Modem_API_AnswerThread();

//UTILITY 
uint8_t Modem_API_ParseToSymbol(char *text, char *parsed, char symbol);
bool Modem_API_CleanBuffer(uint8_t *buf, uint16_t len);

//RESPONSE 
bool Modem_API_AT_Response_CGNSCHK(char *params);
bool Modem_API_AT_Response_CGNSINF(char*params);
bool Modem_API_AT_Response_HTTPACTION();
bool Modem_API_AT_Response_CMTI(char *params);
bool Modem_API_AT_Response_CMGR(char *params);
bool Modem_API_AT_Response_NotParsed(char *params);
bool Modem_API_AT_Response_CCLK(char *params);

//RTOS FUNCTIONS 
bool Modem_API_Init(){ 

	UART_API_Init(eUartModem, eBaudRate9600);
	GPIO_Driver_Init(eGpioPinA6GSMPower, ePinHigh);


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

    if ((pet_tracker_status & WAITING_FOR_MESSAGE) != 0){ 
    	 parsed_message->at_command = eModemATNotParsed;
    	for (i = 0;  i < 30; i++){
        	if (raw_data->buffer_adress[i] == '\n'){
        		parsed_message->command_answer[i] = raw_data->buffer_adress[i];
				return true;
        	}
        	else  parsed_message->command_answer[i] = raw_data->buffer_adress[i];
        }
    }

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
    if (parsed_message->at_command == eModemATLast){
    	parsed_message->at_command = eModemATNotParsed;  //no command answer found on LUT
    	for (int i = 0; i<index; i++){
    		parsed_message->command_answer[i] = temporary [i];
    	}
    	return true;
    }
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
			osMessageQueuePut(modem_answer_queue_id, &modem_message, osPriorityHigh, COMMANDS_QUEUE_PUT_TIMEOUT);
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
                    HardFault_Handler();
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
                case eModemATCGNSCHK:{ 
                    Modem_API_AT_Response_CGNSCHK(parsed_message->command_answer);
                    break;
                }
                case eModemATCGNSINF:{ 
                    Modem_API_AT_Response_CGNSINF(parsed_message->command_answer);
                    break;
                }
                case eModemATCMGR: {
                    Modem_API_AT_Response_CMGR(parsed_message->command_answer);
                    break; 
                }
                case eModemATCMTI: {
                    Modem_API_AT_Response_CMTI(parsed_message->command_answer);
                    break; 
                }
                case eModemATHTTPACTION:{
                	Modem_API_AT_Response_HTTPACTION();
                	break;
                }
                case eModemATNotParsed:{
                	Modem_API_AT_Response_NotParsed(parsed_message->command_answer);
                	break;
                }
            	case eModemATPSUTTZ:{
					break;
				}
            	case eModemATDST:{
            		pet_tracker_status |= MODEM_INITIALISED;
					break;
				}
                case eModemATLast:{
                    break;
                } 
                case eModemATCCLK: { 
                    Modem_API_AT_Response_CCLK(parsed_message->command_answer); 
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




//MODEM COMMANDS (SENT THROUGH DEBUG OR SMS):


bool Modem_API_GetLocation(char *params){
	char modem_tx[20];
	snprintf(modem_tx,20,"AT+CGNSINF\n");
	Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
    if (gnss_status & GNSS_FIX_COMPLETE != 0){ 
        snprintf(modem_tx,20,"AT+CCLK?\n");
        Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
        if (num_of_fixes == 100) num_of_fixes = 0;

        EEPROM_API_SendBuffer(eEepromAT24C256A1, num_of_fixes*33, latitude, 8);
        EEPROM_API_SendBuffer(eEepromAT24C256A1, num_of_fixes*33+8, longitude, 8);
        EEPROM_API_SendBuffer(eEepromAT24C256A1, num_of_fixes*33+16, time_of_fix, 17);
        num_of_fixes++;
        EEPROM_API_SendByte(eEepromAT24C256A1, EE_LAST_FIX, num_of_fixes);
        gnss_status &= ~GNSS_FIX_COMPLETE;
    }
	return true;
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


bool Modem_API_GNSS_Power(char *params){
    uint8_t i=0;
    char power[3]={0};
    char modem_tx[20]={0};
    uint32_t flags=0; 
    i = Modem_API_ParseToSymbol(params, power, '\n' );
    if (i == 0) return false; 
    if (((power[0]-'0') == 1) || ((power[0]-'0') == 0)){
    	power[0] -= '0';
    	snprintf(modem_tx, 20, "AT+CGNSPWR=%d\n", power[0]);
    } else {
    	return false; //unknown value
    }

    if (((power[0] == 0) && ((pet_tracker_status & GNSS_POWER_ON) == 0))
    	||((power[0] == 1) && ((pet_tracker_status & GNSS_POWER_ON) != 0))) return true; //already on or off
    

    Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
    (power[0] == 0) ? (pet_tracker_status &= ~GNSS_POWER_ON) : (pet_tracker_status |= GNSS_POWER_ON);
    if (power[0] == 0) return true;
    if ((pet_tracker_options & AGPS_ENABLE) == 0) return true; //no agps

    //agps 
    Modem_API_CheckEpo(NULL); 
    if (((epo_file_status & 1) == 0) || ((epo_file_status>>8) == 0)){
        Modem_API_DownloadEPO(NULL); //download epo because no file or expired 
        flags = osEventFlagsWait(modem_flags, MODEM_OK|MODEM_ERROR, osFlagsWaitAny, 20000);
        if (flags != osErrorTimeout) Modem_API_CheckEpo(NULL);
    }
    if ((epo_file_status & 1) == 1) {
    	snprintf(modem_tx, 20, "AT+CNTP\n");
    	Modem_API_SendWait(modem_tx, eModemSendWaitCustom, 5000);
        snprintf(modem_tx, 20, "AT+CGNSAID=31,1,1\n");
        Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 2000, MODEM_OK|MODEM_ERROR) ; 
    } else { 
        return false; 
    }
    return true;
}


bool Modem_API_CheckEpo(char *params){
    char modem_tx[] = "AT+CGNSCHK=3,1\n";
    bool stat;
    stat = Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 3000, MODEM_OK);
    if (stat == false) return false;
    return true;
}


bool Modem_API_DownloadEPO(char *params){ 
    char modem_tx[100];
    uint32_t stat;
    snprintf (modem_tx, 50, "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n");
    stat = Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
    if (stat == 0) return false; 

    snprintf (modem_tx, 50, "AT+SAPBR=3,1,\"APN\",\"%s\"\n", pet_tracker_apn); //set apn
    stat = Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
    if (stat == 0) return false;
   
    snprintf (modem_tx, 50, "AT+SAPBR=0,1\n"); //disable bearer
    stat = Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
    if (stat == 0) return false;
    
    snprintf (modem_tx, 50, "AT+SAPBR=1,1\n"); //active bearer
    stat = Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
    if (stat == 0) return false;

    snprintf (modem_tx, 50, "AT+CNTPCID=1\n"); //set gprs bearer id 
    stat = Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
    if (stat == 0) return false;

    snprintf (modem_tx, 50, "AT+CNTP\n");  //synchronize clock
    stat = Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
    if (stat == 0) return false;

    snprintf (modem_tx, 50, "AT+CGNSSAV=3,3\n"); //http save epo 
    stat = Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
    if (stat == 0) return false;

    snprintf (modem_tx, 50, "AT+HTTPINIT\n"); //init http service
    stat = Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
    if (stat == 0) return false;

    snprintf (modem_tx, 50, "AT+HTTPPARA=\"CID\",1\n"); //cid profile 
    stat = Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
    if (stat == 0) return false;

    snprintf (modem_tx, 100, "AT+HTTPPARA=\"URL\",%s\n", EPO_DOWNLOAD_LINK); //set download link 
    stat = Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
    if (stat == 0) return false;

    snprintf (modem_tx, 50, "AT+HTTPACTION=0\n");  // start download
    stat = Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
    if (stat == 0) return false;

    return true;
}



bool Modem_API_ReadSMS(char *params){ 
	char id[3] = {0};
	char modem_tx[30] = {0};
	uint8_t id_index = 0;

	for (uint8_t start = 0, i = 0; i < 5 ; i++){
		if ((params[i] >= '0') && (params[i] <= '9')){
			if (start == 0) start = 1;
			id[id_index++] = params[i];
		} else {
			if (start == 1) break;
			if (i == 4) return false;
		}
	}

	if (id_index == 0) return false;

	snprintf(modem_tx, 30, "AT+CMGF=1\n");
	Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);

	snprintf(modem_tx, 30, "AT+CMGR=%s\n", id);
	Modem_API_SendWait(modem_tx, eModemSendWaitNo);

    return true;
}

bool Modem_API_DeleteSMS(char *params){
    char temp[5]; 
    uint8_t length = Modem_API_ParseToSymbol((params), temp, '\n');
    if (length == 1) length = temp[0] - '0';
    else if (length == 2) length = 10*(temp[0] - '0') + (temp[1] - '0');
    else if (length == 3) length = 100*(temp[0] - '0') + 10*(temp[1] - '0') + (temp[2] - '0');
    else return false;
    
    char modem_tx[20]={0};
    if (length == 255) snprintf(modem_tx, 20, "AT+CMGD=%d,%d\n",1, 4);
    else snprintf(modem_tx, 20, "AT+CMGD=%d\n", length);
    Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK | MODEM_ERROR );
    return true;
}


bool Modem_API_SendLocation(char *params){ 
    char temp[5];
    char message_text[100]={0};
    uint8_t length = Modem_API_ParseToSymbol((params), temp, '\n');
    if (length == 1) length = temp[0] - '0';
    else if (length == 2) length = 10*(temp[0] - '0') + (temp[1] - '0');
    else return false;
    snprintf(message_text, 100, "Time:%s Place:https://google.com/maps/place/%s,%s", time_of_fix, latitude, longitude);
    
    
    char *command = calloc(100, sizeof(char));
	snprintf(command, 100, "%s,%s\n", allowed_contacts[length-1] , message_text);
	CMD_API_PuttoQueue(eCommandModulesModem, eModemCommandsSendSMS, command, 100);
    free(command);

}









//--------------------------------------------------------------------------

//MODEM AT COMMANDS RESPONSE FUNCTIONS (WHEN MODEM SENDS ANSWERS): 

bool Modem_API_AT_Response_CGNSCHK(char *params){
    char temp[10]={0};
    uint8_t i=1;
    uint8_t length=0;

    /*should send something like this 3,1,27456,xx  3 is to indicate epo
     1 is epo exists 0 doesnt exists, next is size last one is available hours*/
    //epo file parse
    length = Modem_API_ParseToSymbol((params+i), temp, ',');
    if (temp[0]-'0' != 3){
    	epo_file_status = 0;
    	return false; // somethings wrong no file
    }
    Modem_API_CleanBuffer((uint8_t*)temp, 10);
    i+=length+1;


    //exist or not
    length = Modem_API_ParseToSymbol((params+i), temp, ',');
    if (temp[0]-'0' != 1){
        epo_file_status = 0;
        return true; // no epo file, need to download
    }
    Modem_API_CleanBuffer((uint8_t*)temp, 10);
    i+=length+1;

    //size - doesnt matter
    length = Modem_API_ParseToSymbol((params+i), temp, ',');
    Modem_API_CleanBuffer((uint8_t*)temp, 10);

    length = Modem_API_ParseToSymbol((params+i), temp, ',');
    (temp[0] > '0') ?  (temp[0]-='0') : (temp[0] = 0);
    (temp[1] > '0') ?  (temp[1]-='0') : (temp[1] = 0);
    uint8_t hours = temp[0]*10+temp[1];
    if (hours != 0){
    	epo_file_status = 0;
    	epo_file_status |= (hours<<8);
    	epo_file_status |= 1U;
    } else  {
    	epo_file_status = 0;
    }
    return true; 
}


bool Modem_API_AT_Response_CGNSINF(char*params){
	char temp[20] = {0};
	uint8_t i=1;
	uint8_t length=0;
	length = Modem_API_ParseToSymbol((params+i), temp, ',');
    if (length == 255) return true;
	i+=length+1;
    Modem_API_CleanBuffer((uint8_t*)temp, 20);
	length = Modem_API_ParseToSymbol((params+i), temp, ',');
    if (length == 255) return true;
    if (temp[0] == '0'){
    	gnss_status &= ~GNSS_FIX_COMPLETE;//clear gnss  fix flag
    } else {
    	gnss_status |= GNSS_FIX_COMPLETE; //add gnss fix flag
    }
    if(
        (
    		((gnss_status & GNSS_FIX_COMPLETE) == 0)
    	    &&((pet_tracker_options & GNSS_FULL_PARSE) != 0)
		)	//no fix but full parse etc.
    	||((gnss_status & GNSS_FIX_COMPLETE) != 0)
    ){

		i+=length+1;
		Modem_API_CleanBuffer((uint8_t*)temp, 20);
		length = Modem_API_ParseToSymbol((params+i), temp, ',');
		if (length == 255) return true;
		i+=length+1;

		Modem_API_CleanBuffer((uint8_t*)temp, 20);
		length = Modem_API_ParseToSymbol((params+i), temp, ',');
		if (length == 255) return true;
		snprintf(latitude, length, temp);
		i+=length+1;
 		Modem_API_CleanBuffer((uint8_t*)temp, 20);
		length = Modem_API_ParseToSymbol((params+i), temp, ',');
		if (length == 255) return true;
		snprintf(longitude, length, temp);
    }

	return true;
}


bool Modem_API_AT_Response_HTTPACTION(){ 
    char modem_tx[100];
    uint32_t stat;
    Modem_API_CheckEpo(NULL);  //refresh epo flags
    snprintf (modem_tx, 50, "AT+HTTPTERM\n");  //terminate http 
    stat = Modem_API_SendWait(modem_tx, eModemSendWaitFlagCustom, 1000, MODEM_OK|MODEM_ERROR);
    if (stat == 0) return false;
    else return true;
}


bool Modem_API_AT_Response_CMTI(char *params){ 
	uint8_t sms_id = 0;
	char sms[5]={0};
	uint8_t sms_index = 0;


	for (uint8_t start = 0, i=0; i<20 ; i++){
		if ((params[i] >= '0') && (params[i] <= '9')){
			if (start == 0) start = 1;
			sms[sms_index++] = params[i];
		} else {
			if (start == 1) break;
		}
	}

	if (sms_index == 1) sms_id = (sms[0] - '0');
	if (sms_index == 2) sms_id = (((sms[0] - '0')*10) + (sms[1] - '0'));



    char *command = calloc(20, sizeof(char));
    CMD_API_PuttoQueue(eCommandModulesModem,eModemCommandsReadSMS, command, 20);
    free (command);
	return true;
}


bool Modem_API_AT_Response_CMGR(char *params){

    pet_tracker_status |= WAITING_FOR_MESSAGE; 

    return true;
}


bool Modem_API_AT_Response_NotParsed(char *params){
    if ((pet_tracker_status & WAITING_FOR_MESSAGE) != 0) {
        char *command = calloc(100, sizeof(char));
        for (int i = 0; i < 100 ; i++){
        	if (params[i] != 0){
        		command[i] = params[i];
        	} else{
        		command[i]='\n';
        		break;
        	}
        }
        UART_API_PutToQueue(eUartDebug, command);
        pet_tracker_status &= ~WAITING_FOR_MESSAGE;
    }


	return true;
}

bool Modem_API_AT_Response_CCLK(char *params){ 
    snprintf(time_of_fix, 18, params+2);
    return true;
}


//UTILITY-----------------------------------------------------------------------

uint32_t Modem_API_SendWait(char * params, eModemSendWait modem_wait, ...){
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
        return 0; //mutex is not free
    } else {
        if ((modem_wait == eModemSendWaitFlagCustom) 
            ||(modem_wait == eModemSendWaitFlagForever)){
                osEventFlagsClear(modem_flags, wait_flag);
        }
        UART_API_SendString(eUartModem, params, 250);
        switch (modem_wait) {
        	case eModemSendWaitNo: {
        		osMutexRelease(modem_mutex_id);
        		return 0;
        	}
        	case eModemSendWaitCustom: {
        		osDelay(wait_time);
        		osMutexRelease(modem_mutex_id);
        		return 0;
        	}
        	case eModemSendWaitFlagCustom:
        	case eModemSendWaitFlagForever:{
				got_flag = osEventFlagsWait(modem_flags, wait_flag, osFlagsWaitAny, wait_time);
        		osMutexRelease(modem_mutex_id);
        		return (got_flag != osFlagsErrorTimeout) ? got_flag : 0;
        	}
        	default: {
        		osMutexRelease(modem_mutex_id);
        		return 0;
        	}
        }
    }
    return 0; 
}


uint8_t Modem_API_ParseToSymbol(char *text, char *parsed, char symbol) {
	uint8_t length = 0;
	uint8_t start = 0;
	for (int i = 0; i<255; i++){
		if ((text[i] >= '0' && text[i] <= '9') || text[i] == '.' || text[i] == '\r') {
			if (start == 0) start = 1; //number start
			if (text[i] != 13) parsed[length++] = text[i];
		} else if ((text[i] == symbol)) break; //number parsed succesfully
		else if (start == 1) return 0xFF; //number is split or something wrong
	}
	return length;
}


bool Modem_API_CleanBuffer(uint8_t *buf, uint16_t len) {
	for (uint8_t i = 0; i<len; i++){
		buf[i] = 0;
	}
	return true;
}





