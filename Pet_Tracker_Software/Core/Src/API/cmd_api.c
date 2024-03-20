#include "cmd_api.h"
#include "cli_app.h"
#include "cmsis_os.h"
#include "uart_api.h"
#include "string.h"

#include "power_api.h"
#include "cli_function_list.h"

#define COMMAND_QUEUE_SIZE 10
#define COMMANDS_QUEUE_PUT_TIMEOUT 100
#define MODULE_MAX_NAME 20
//RTOS VARIABLES 
const osThreadAttr_t cmd_api_thread_atr = {
    .name = "cmd_api_thread",
    .stack_size = 1024,
    .priority = osPriorityHigh
};
osThreadId_t cmd_api_thread_id = NULL;
const osMessageQueueAttr_t commands_queue_atr = {.name = "commands_queue"};
osMessageQId commands_queue_id;



//DATA VARIABLES
sUartData_t uart_data_unproccessed = {0}; 
sCommandParameters_t * command_params = NULL;
typedef struct sCommandParserTempString{ 
    char text[50]; 
    int  index;
} sCommandParserTempString; 
sCommandParserTempString TEMP; 
const char command_module_names_lut[eCommandModulesLast][MODULE_MAX_NAME] = {
    [eCommandModulesModem]  = "MODEM",
    [eCommandModulesLED]    = "LED",
    [eCommandModulesPCUART] = "PCUART",
    [eCommandModulesPower]  = "POWER",
    [eCommandModulesAcce]   = "ACCE",
    [eCommandModulesEEPROM] = "EEPROM"
};



const sCommandFunctions_t * command_function_all_modules[eCommandModulesLast] = {
    [eCommandModulesModem]  = modem_command_function_lut,
    [eCommandModulesLED]    = NULL,
    [eCommandModulesPCUART] = NULL,
    [eCommandModulesPower]  = NULL,
    [eCommandModulesAcce]   = NULL,
    [eCommandModulesEEPROM] = NULL
};

const uint8_t command_function_all_modules_sizes[eCommandModulesLast] = {
	[eCommandModulesModem]  = (uint8_t)(eModemCommandsLast),
	[eCommandModulesLED]    = 0,
	[eCommandModulesPCUART] = 0,
	[eCommandModulesPower]  = 0,
	[eCommandModulesAcce]   = 0,
	[eCommandModulesEEPROM] = 0

};





//FUCTIONS 
bool CMD_API_CommandParser(sCommandParameters_t*  parse_command, sUartData_t * uart_data);
void CMD_API_Thread (void *argument);
eCommandModules_t CMD_API_ModuleParser(char * module_name);
uint8_t CMD_API_InstructionParser(char * command_name, eCommandModules_t module);




//CMD THREAD INIT 
bool CMD_API_ThreadInit(void) {
    if  (commands_queue_id == NULL) {
        commands_queue_id = osMessageQueueNew(COMMAND_QUEUE_SIZE, sizeof(sCommandParameters_t), &commands_queue_atr);
        if (commands_queue_id == NULL) {
        	return false;
        } 
    }

    if (cmd_api_thread_id == NULL){
		cmd_api_thread_id = osThreadNew(CMD_API_Thread, NULL, &cmd_api_thread_atr);
		if (cmd_api_thread_id == NULL) return false;
    }
    return true;  
}


/*CMD THREAD. THIS FUNCTION WAITS INCOMING UART DATA, THEN CALLS CUT FUNCTION TO PARSE COMMAND STRUCTURE AND PUTS
  IT INTO COMMANDS QUEUE*/
void CMD_API_Thread (void *argument) {
    while(1){ 
        if (UART_API_GetMessage(&uart_data_unproccessed, osWaitForever) == true) {
			command_params = calloc(1, sizeof(sCommandParameters_t));
			command_params -> params = calloc(50, sizeof(char));
			CMD_API_CommandParser(command_params, &uart_data_unproccessed);
			osMessageQueuePut(commands_queue_id, &command_params, osPriorityHigh, COMMANDS_QUEUE_PUT_TIMEOUT);
			free(uart_data_unproccessed.buffer_adress);
        }
    }
    osThreadTerminate(cmd_api_thread_id);
}



//THIS FUNCTION PARSES COMMANDS, MODULES, AND FUNCTIONS NUMBER FROM RAW UART DATA

bool CMD_API_CommandParser(sCommandParameters_t*  parse_command, sUartData_t * uart_data) {
    uint8_t no_params = 0; 
    //CHECK POINTER
    if (uart_data->buffer_adress == NULL) return false;
    for ( ; TEMP.index > 0; TEMP.index--){
            TEMP.text[TEMP.index] = 0;
        }
    int i = 0;
    //PARSEMODULE. GO THROUGH TEXT AND CHECK IF NO 0, NO \N AND NOT LONGER THAN MAX MODULE NAME
    while (1){         
        TEMP.text[TEMP.index] = *(uart_data->buffer_adress+i); 
        if( 
            (TEMP.text[TEMP.index] == 0) 
            ||(TEMP.text[TEMP.index] == '\n')
            ||(i==MODULE_MAX_NAME) 
        ) return false; 
        if (TEMP.text[TEMP.index] == ':') break; //MODULE PARSE TERMINATION CHARACTER
        else { 
            i++; 
            TEMP.index++;
        }
    }
    parse_command->module = CMD_API_ModuleParser(TEMP.text); //PARSE FROM TEXT. 
    if (parse_command->module == eCommandModulesERROR) return false;  //NO MODULE
    //CLEAN TEMP BUFFER
    for ( ; TEMP.index > 0; TEMP.index--){
        TEMP.text[TEMP.index] = 0; 
    } 
    i++;
    //PARSE COMMAND NAME TO LOOK THROUGH TABLES
    while (1){ 
        TEMP.text[TEMP.index] = *(uart_data->buffer_adress+i); 
        if(
            (TEMP.text[TEMP.index] == 0) 
            ||(TEMP.index == MODULE_MAX_NAME)
        )   return false; 
        if (TEMP.text[TEMP.index] == '\n'){
            no_params = 1; 
            break; //END OF COMMAND NAME AND NO PARAMS WILL BE USED 
        } else if (TEMP.text[TEMP.index] == ':') //COMMAND PARSE TERMINATION COMMAND
            break; 
        else {
            i++; 
            TEMP.index++; 
        } 
    }
    parse_command->command = CMD_API_InstructionParser(TEMP.text, parse_command->module);
    if (parse_command->command == 0xFF) return false;
    //CLEAN TEMP BUFFER
    for ( ; TEMP.index > 0; TEMP.index--){
        TEMP.text[TEMP.index] = 0; 
    } 
    i++;
    //PARSE COMMAND PARAMETERS
    if (no_params == 0){
        while (1){
            TEMP.text[TEMP.index] = *(uart_data->buffer_adress+i);
            if((TEMP.text[TEMP.index] == 0) || (TEMP.index == 50)) return false;
            else if (TEMP.text[TEMP.index] == '\n'){
                strncpy(parse_command ->params, TEMP.text, 50);
                break;
            } else {
            	i++;
            	TEMP.index++;
            }
        }
    }
    return true;
}

//THIS FUNCTION PARSE MODULE ENUM 

eCommandModules_t CMD_API_ModuleParser(char * module_name){ 
    char t[MODULE_MAX_NAME] = {0};
    uint8_t t_index = 0;  
    for (int i=0; i<MODULE_MAX_NAME; i++) { 
        if (((module_name[i] >= 'A') && (module_name[i]<='Z')) || ((module_name[i] >= 'a') && (module_name[i]<='z')) ){
            if ((module_name[i] >= 'a') && (module_name[i]<='z')) module_name[i] -=32; //convert to uppercase 
            t[t_index++] = module_name[i];  
        } else if (module_name[i] == ':') break; 
        else t[t_index] = 0; 
    }
    for (eCommandModules_t index =eCommandModulesFirst; index < eCommandModulesLast; index++) {
        if(strncmp(t, command_module_names_lut[index], MODULE_MAX_NAME) == 0){
            return index; 
        }
    }
    return eCommandModulesERROR;     
}

//THIS FUNCTION PARSES ACTUAL COMMAND FOR MODULE
  
uint8_t CMD_API_InstructionParser (char * command_name, eCommandModules_t module){ 
    char t[MODULE_MAX_NAME] = {0};
    uint8_t t_index = 0;  
    const sCommandFunctions_t * functions_lut = command_function_all_modules[module];
    uint8_t functions_lut_size = command_function_all_modules_sizes[module];


    for (int i=0; i<MODULE_MAX_NAME; i++) { 
        if (((command_name[i] >= 'A') && (command_name[i]<='Z')) || ((command_name[i] >= 'a') && (command_name[i]<='z'))){
            if ((command_name[i] >= 'a') && (command_name[i]<='z')) command_name[i]-=32; //convert to UPPER CASE 
            t[t_index++] = command_name[i];  
        } else if (command_name[i] == ':') break; 
        else t[t_index] = 0; 
    }
    
    for (uint8_t i = 0; i<functions_lut_size; i++) {
        if (strncmp(t, (functions_lut+i)->name, MODULE_MAX_NAME) == 0) return i; //COMMAND FOUND IN LUT
    }

    return 0xFF; //NO COMMAND





} 

