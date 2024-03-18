#include "cmd_api.h"
#include "cli_app.h"
#include "cmsis_os.h"
//#include "debug_api.h"



//RTOS VARIABLES 
const osThreadAttr_t cmd_api_thread_atr = {
    .name = "cmd_api_thread",
    .stack_size = 1024,
    .priority = osPriorityHigh
};
osThreadId_t cmd_api_thread_id = NULL;

bool CMD_API_MessageCut (sCommandParameters_t *cli_app_message_buffer);



void CMD_API_Thread (void *argument) {
    while(1){ 
        osDelay(500);
    }
    osThreadTerminate(cmd_api_thread_id);
}


bool CMD_API_ThreadInit(void) {
    cmd_api_thread_id = osThreadNew(CMD_API_Thread, NULL, &cmd_api_thread_atr);
    if (cmd_api_thread_id == NULL) return false; 
    return true;  
}

bool CMD_API_MessageCut (sCommandParameters_t *cli_app_message_buffer) {
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

