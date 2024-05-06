/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "cli_function_list.h"
#include "gpio_driver.h"
#include "string.h"
#include "stdlib.h"


#include "modem_api.h"
#include "power_api.h"



bool Misc_Command_WritePin(char * params);
bool Misc_Command_InitPin(char * params);
bool Misc_Command_TogglePin(char * params);




const sCommandFunctions_t modem_command_function_lut[eModemCommandsLast] = {
	[eModemCommandsSendCommand] = {.name = "SEND",			.function_pointer = &Modem_API_SendCommand},
	[eModemCommandsGNSSPower]	= {.name = "GNSSPOWER", 	.function_pointer = &Modem_API_GNSS_Power},
	[eModemCommandsSendSMS]		= {.name = "SMS", 			.function_pointer = &Modem_API_SendSMS},
	[eModemCommandsDownloadEPO] = {.name = "DOWNLOADEPO",   .function_pointer = &Modem_API_DownloadEPO},
	[eModemCommandsCheckEPO]	= {.name = "CHECKEPO", 		.function_pointer = &Modem_API_CheckEpo}, 
	[eModemCommandsReadSMS]		= {.name = "READSMS", 		.function_pointer = &Modem_API_ReadSMS},
	[eModemCommandsDeleteSMS]	= {.name = "DELETESMS",     .function_pointer = &Modem_API_DeleteSMS},
	[eModemCommandsGetLocation] = {.name = "GETLOCATION",   .function_pointer = &Modem_API_GetLocation},
	[eModemCommandSendLocation]	= {.name = "SENDLOCATION",  .function_pointer = &Modem_API_SendLocation}
};
const sCommandFunctions_t led_command_function_lut[eLEDCommandsLast]={
	[eLEDCommandsFirst]	= {.name = "A", .function_pointer = NULL}
};
const sCommandFunctions_t pcuart_command_function_lut[ePCUARTCommandsLast]={
	[ePCUARTCommandsFirst]	= {.name = "A", .function_pointer = NULL}
};
const sCommandFunctions_t power_command_function_lut[ePowerCommandsLast]={
	[ePowerCommands4VPower]		= {.name = "4VPOWER", .function_pointer =&POWER_API_4VPower},
	[ePowerCommandsLEDPower]	= {.name = "LEDPOWER", .function_pointer =&POWER_API_LEDPower},
	[ePowerCommandsModemPower]	= {.name = "GSMPOWER", .function_pointer =&POWER_API_ModemPower}
};
const sCommandFunctions_t acce_command_function_lut[eAcceCommandsLast]={
	[eAcceCommandsFirst]	= {.name = "A", .function_pointer = NULL}												
};
const sCommandFunctions_t eeprom_command_function_lut[eEEPROMCommandsLast]={
	[eEEPROMCommandsFirst]	= {.name = "A", .function_pointer = NULL}	
};

const sCommandFunctions_t misc_command_function_lut[eMiscCommandsLast] = { 
	[eMiscCommandsWritePin] 	= {.name = "WRITEPIN", .function_pointer = &Misc_Command_WritePin}, 
	[eMiscCommandsInitPin]		= {.name = "INITPIN", .function_pointer = &Misc_Command_InitPin}, 
	[eMiscCommandsTogglePin] 	= {.name = "TOGGLEPIN", .function_pointer = &Misc_Command_TogglePin}, 
};




bool gpio_pin_parser (eGpioPin_t *gpio_p, eGpioPinState_t *pin_sta, char* params){
	eGpioPin_t gpio_pin; 
	eGpioPinState_t pin_state; 
	uint8_t i = 0; 
	uint8_t temp;
	char t[10]={0};
	uint8_t t_index = 0; 
	for (i = 0; i<50; i++, t_index++){ 
		if(i == 5 || params[i] == 0) return false; //should not reach here if ok 
		else { 
			if (params[i] == ',') break;
			if ((params[i] >= '0') && (params[i] <= '9')) t[t_index] = (params[i] - '0'); 
		}
	}
	if (t_index > 2) return false; //max allowed two digits
	if (t_index == 1) temp = t[0];  //one digits
	else temp = t[0] * 10 + t[1];  //two digits  
	gpio_pin = (eGpioPin_t)temp; 
	if (gpio_pin >= eGpioPinLast) return false; //not good parameter, too big pin number
	i++; //because break in previous for loop

	t[0] = t[1] = t_index = 0; 
	for ( ; i<50; i++, t_index++){ 
		if (i>10 || params[i] == 0) return false; //should not reach here if ok
		else {
			if (params[i] >= '0' && params[i] <= '9'){
			 	t[t_index] = (params[i] - '0');
				break; 
			}
		}
	} 
	if (t[0] == 1) pin_state = ePinHigh;
	else if (t[0] == 0) pin_state = ePinLow;
	else pin_state = ePinNaN;
	if (pin_state>=ePinNaN) return false; //noot good parameter not 1 or 0

	//everythings good, params parsed
	*gpio_p = gpio_pin; 
	*pin_sta = pin_state; 
	return true; 
}






bool Misc_Command_WritePin (char *params){
	eGpioPin_t gpio_pin; 
	eGpioPinState_t pin_state; 
	if (gpio_pin_parser(&gpio_pin, &pin_state, params)!=true) return false; 
	if ((gpio_init_status & (1<<gpio_pin)) == 0) return false;
	if (GPIO_Driver_WritePin(gpio_pin, pin_state) != true) return false; 
	return true; 



}

bool Misc_Command_InitPin (char *params){
	eGpioPin_t gpio_pin; 
	eGpioPinState_t pin_state; 
	if (gpio_pin_parser(&gpio_pin, &pin_state, params)!=true) return false;
	if (GPIO_Driver_Init(gpio_pin, pin_state) != true) return false; 
	return true; 
}

bool Misc_Command_TogglePin (char *params){
	eGpioPin_t gpio_pin; 
	eGpioPinState_t pin_state; 
	if (gpio_pin_parser(&gpio_pin, &pin_state, params)!=true) return false;
	if ((gpio_init_status & (1<<gpio_pin)) == 0) return false;
	if (GPIO_Driver_TogglePin(gpio_pin) != true) return false; 
	return true; 
}



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





