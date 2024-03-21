/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "cli_function_list.h"
#include "string.h"
#include "stdlib.h"


#include "modem_api.h"
#include "power_api.h"

const sCommandFunctions_t modem_command_function_lut[eModemCommandsLast] = {
	[eModemCommandsPower] 		= {.name = "POWER",			.function_pointer = &Modem_API_Power},
	[eModemCommandsGNSSPower]	= {.name = "GNSSPOWER", 	.function_pointer = &Modem_API_GNSS_Power},
};
const sCommandFunctions_t led_command_function_lut[eLEDCommandsLast]={
	[eLEDCommandsFirst]	= {.name = "A", .function_pointer = NULL}
};
const sCommandFunctions_t pcuart_command_function_lut[ePCUARTCommandsLast]={
	[ePCUARTCommandsFirst]	= {.name = "A", .function_pointer = NULL}
};
const sCommandFunctions_t power_command_function_lut[ePowerCommandsLast]={
	[ePowerCommandsFirst]	= {.name = "4VPOWER", .function_pointer =&POWER_API_4VPower},
	[ePowerCommandsFirst]	= {.name = "GNSSPOWER", .function_pointer =&POWER_API_GNSSPower},
	[ePowerCommandsFirst]	= {.name = "LEDPOWER", .function_pointer =&POWER_API_LEDPower},
	[ePowerCommandsFirst]	= {.name = "GSMPOWER", .function_pointer =&POWER_API_ModemPower}
};
const sCommandFunctions_t acce_command_function_lut[eAcceCommandsLast]={
	[eAcceCommandsFirst]	= {.name = "A", .function_pointer = NULL}												
};
const sCommandFunctions_t eeprom_command_function_lut[eEEPROMCommandsLast]={
	[eEEPROMCommandsFirst]	= {.name = "A", .function_pointer = NULL}	
};




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





