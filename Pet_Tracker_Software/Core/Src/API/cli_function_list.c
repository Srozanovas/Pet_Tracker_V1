/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "cli_function_list.h"
//#include "modem_api.h"
//#include "cli_app.h"
#include "string.h"
#include "stdlib.h"
//#include "debug_api.h"
//#include "accelerometer_api.h"
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define NAME(string) .name=#string , .name_length = sizeof(#string)
#define MAX_LENGTH 500
/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

const sFunctionName_t function_lut_cli[] = {
	[eFunctionListCLIModemSendCommand] = {NAME(modem_send_command), .function_pointer = &CMD_API_ModemSendCommand},
	[eFunctionListCLIModemSendSMS] = {NAME(modem_send_sms), .function_pointer = &CMD_API_ModemSendSMS},
	[eFunctionListCLIModemReadSMS] = {NAME(modem_read_sms), .function_pointer = &CMD_API_ModemReadSMS},
	[eFunctionListCLIModemConfig]  = {NAME(modem_config), .function_pointer = &CMD_API_ModemConfig},
};






/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables and references
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of private functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Definitions of private functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/



// bool CMD_API_ModemSendCommand (sCommandResponse_t *command_response) {
//     if (Modem_API_SendCommand(command_response->message_args, command_response->args_length) == false) {
//         snprintf(command_response->response_buffer,ERROR_BUFFER_SIZE, "Can't send command to modem");
//         return false;
//     }
//     return true;
// }

// bool CMD_API_ModemSendSMS (sCommandResponse_t *command_response) {
//     message_sent_flag = 0;
//     uint16_t size = 0;
//     char *buffer = NULL;
//     char *number = NULL;
//     number = strtok(command_response->message_args, ",");
//     size = strnlen(number,MAX_LENGTH)-1;
//     if (number == NULL || size == MAX_LENGTH || size == 0){
//         snprintf(command_response->response_buffer, ERROR_BUFFER_SIZE, "Didn't receive number\n");
//         return false;
//     }
//     buffer = number + size +2;
//     size = strnlen(buffer, MAX_LENGTH)-1;
//     if (buffer == NULL || size == MAX_LENGTH-1 || size == 0) {
//         snprintf(command_response->response_buffer, ERROR_BUFFER_SIZE, "Didn't receive text\n");
//         return false;
//     }
//     if (Modem_API_SendSMS(number, buffer, size, command_response->response_buffer) != true) {

//         return false;
//     }
//     message_sent_flag = 1;
//     return true;
// }

// bool CMD_API_ModemReadSMS(sCommandResponse_t *command_response){


// 	return true;
// }

// bool CMD_API_ModemConfig(sCommandResponse_t *command_response){
//     ACC_API_Suspend();
//     if (Modem_API_Config()!= true){
//         Modem_API_Config();
//     }
//     ACC_API_Resume();

//     return true;
// }





