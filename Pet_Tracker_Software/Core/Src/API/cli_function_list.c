/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "cli_function_list.h"
#include "string.h"
#include "stdlib.h"


#include "modem_api.h"

const sCommandFunctions_t modem_command_function_lut[eModemCommandsLast] = {
	[eModemCommandsPower] 		= {.name = "POWER",			.function_pointer = &Modem_API_Power},
	[eModemCommandsGNSSPower]	= {.name = "GNSSPOWER", 	.function_pointer = &Modem_API_GNSS_Power},
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





