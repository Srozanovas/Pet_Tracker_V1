/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
//#include "debug_api.h"
#include "cmd_api.h"
#include "cli_app.h"
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
//DEBUG_MODULE(CLI_APP);
/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
const osThreadAttr_t cli_app_thread_atr = {
    .name = "cli_app_thread",
    .stack_size = 1024,
    .priority = osPriorityNormal
};

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
osThreadId_t cli_app_thread_id = NULL;
sUartData_t message_buffer = {0};
sCommandParameters_t cli_app_command_struct = {0};

/**********************************************************************************************************************
 * Exported variables and references
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of private functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Definitions of private functions
 *********************************************************************************************************************/
bool CLI_APP_Init (void) {
    cli_app_thread_id = osThreadNew(CLI_APP_Thread, NULL, &cli_app_thread_atr);
    if (cli_app_thread_id == NULL) {
        //error("Cannot create cli app thread");
        return false;
    }
    return true;
}

void CLI_APP_Thread (void *argument) {
    while (true) {
        if (UART_API_GetMessage(&message_buffer, osWaitForever) == true) {
            // //ACC_API_Suspend();
            // cli_app_command_struct.buffer_adress = message_buffer.buffer_adress;
            // cli_app_command_struct.buffer_size = message_buffer.size;
            // cli_app_command_struct.functions_lut = function_lut_cli;
            // //cli_app_command_struct.response_buffer = calloc(ERROR_BUFFER_SIZE, sizeof(char));
            // cli_app_command_struct.functions_lut_size = eFunctionListCLILast;
            // if (CMD_API_MessageCut(&cli_app_command_struct) == false) {
            //         //error("%s", cli_app_command_struct.response_buffer);
            // } else {
            //     //if (strlen(cli_app_command_struct.response_buffer) > 0){
            //         //debug("%s", cli_app_command_struct.response_buffer);
            //     //}
            // }
            // //ACC_API_Resume();
            // free(cli_app_command_struct.buffer_adress);
            // free(cli_app_command_struct.response_buffer);
        }
    }
    osThreadTerminate(cli_app_thread_id);
}
/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
