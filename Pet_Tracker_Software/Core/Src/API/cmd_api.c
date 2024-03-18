#include "cmd_api.h"
#include "cli_app.h"
#include "cmsis_os.h"
#include "uart_api.h"
#include "string.h"
//#include "debug_api.h"

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




//FUCTIONS 
bool CMD_API_CommandParser(sCommandParameters_t*  parse_command, sUartData_t * uart_data);
void CMD_API_Thread (void *argument);
eCommandModules_t CMD_API_ModuleParser(char * module_name);



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
        UART_API_GetMessage(&uart_data_unproccessed, osWaitForever);
        command_params = calloc(1, sizeof(sCommandParameters_t)); 
        CMD_API_CommandParser(command_params, &uart_data_unproccessed);
        osMessageQueuePut(commands_queue_id, &command_params, osPriorityHigh, COMMANDS_QUEUE_PUT_TIMEOUT);
        free(uart_data_unproccessed.buffer_adress); 
    }
    osThreadTerminate(cmd_api_thread_id);
}





bool CMD_API_CommandParser(sCommandParameters_t*  parse_command, sUartData_t * uart_data) {
    for (int i=0; i<uart_data->size; i++) {      
        while (1){
            if (uart_data->buffer_adress == NULL) return false;
        	if (*(uart_data->buffer_adress+i) != '\n'){ //not end of message
                while (1){
                    //PARSEMODULE  
                    TEMP.text[TEMP.index++] = *(uart_data->buffer_adress+i); 
                    if ((TEMP.text[TEMP.index-1] == ':') || i == MODULE_MAX_NAME) break;
                    else i++;
                }
                parse_command->module = CMD_API_ModuleParser(TEMP.text);
                break;

            }

        }
        break;
    }


    
    
    
    // sCommandResponse_t command_response_struct = {0};
    // command_response_struct.response_buffer = cli_app_message_buffer->response_buffer;
    // bool found_command = false;
    // uint8_t cmd = 0;
    // if (cli_app_message_buffer == NULL) {
    //     return false;
    // }
    // command_response_struct.message_command = strtok(cli_app_message_buffer->buffer_adress, ":,");
    // if (command_response_struct.message_command == NULL) {
    //     snprintf(cli_app_message_buffer->response_buffer, ERROR_BUFFER_SIZE, "Invalid command name: %s", cli_app_message_buffer->buffer_adress);
    //     return false;
    // }
    // command_response_struct.command_length = strlen(command_response_struct.message_command) + 1;
    // command_response_struct.message_args = command_response_struct.message_command + command_response_struct.command_length;
    // command_response_struct.args_length = cli_app_message_buffer->buffer_size - command_response_struct.command_length;
    // for (cmd = 0; cmd < cli_app_message_buffer->functions_lut_size; cmd++) {
    //     if (command_response_struct.command_length != cli_app_message_buffer->functions_lut[cmd].name_length) {
    //         continue;
    //     }
    //     if (String_Compare(cli_app_message_buffer->functions_lut[cmd].name, command_response_struct.message_command, command_response_struct.command_length) == true) {
    //         found_command = true;
    //         break;
    //     }
    // }
    // if (found_command == true) {
    //     if (cli_app_message_buffer->functions_lut[cmd].function_pointer == NULL) {
    //         snprintf(cli_app_message_buffer->response_buffer, ERROR_BUFFER_SIZE,
    //         "Function pointer = NULL (command: %s: %s)", cli_app_message_buffer->functions_lut[cmd].name, command_response_struct.message_args);
    //         return false;
    //     }
    //     if (cli_app_message_buffer->functions_lut[cmd].function_pointer(&command_response_struct) == false) {
    //         return false;
    //     }
    // } else {
    //     snprintf(cli_app_message_buffer->response_buffer, ERROR_BUFFER_SIZE, "Command not found: %s : %s \n", command_response_struct.message_command, command_response_struct.message_args);
    //     return false;
    // }
    return true;
}




eCommandModules_t CMD_API_ModuleParser(char * module_name){ 
    char t[MODULE_MAX_NAME] = {0};
    uint8_t t_index = 0;  
    for (int i=0; i<MODULE_MAX_NAME; i++) { 
        if ((module_name[i] >= 'A') && module_name[i]<='Z'){
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
