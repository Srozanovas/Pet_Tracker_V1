/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "cli_function_list.h"
#include "gpio_driver.h"
#include "string.h"
#include "stdlib.h"
#include "eeprom_api.h"
#include "uart_api.h"
#include "stdio.h"


#include "modem_api.h"
#include "power_api.h"



bool Misc_Command_WritePin(char *params);
bool Misc_Command_InitPin(char *params);
bool Misc_Command_TogglePin(char *params);
bool Misc_Command_LOG(char *params);
bool Misc_Command_OptionsSave(char *params);



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
	[eMiscCommandsWritePin] 	= {.name = "WRITEPIN", 		.function_pointer = &Misc_Command_WritePin}, 
	[eMiscCommandsInitPin]		= {.name = "INITPIN", 		.function_pointer = &Misc_Command_InitPin}, 
	[eMiscCommandsTogglePin] 	= {.name = "TOGGLEPIN", 	.function_pointer = &Misc_Command_TogglePin}, 
	[eMiscCommandsSave]			= {.name = "OPTIONSSAVE", 	.function_pointer = &Misc_Command_OptionsSave}, 
	[eMiscCommandsLog]			= {.name = "LOG", 			.function_pointer = &Misc_Command_LOG},
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


bool Misc_Command_LOG(char *params){
	uint8_t log_tx[100];
	snprintf(log_tx,24, "Number Of fixes: %d\n", num_of_fixes); 
	UART_API_SendString(eUartDebug, (char*)log_tx, 20);

	for (int i=1; i<=num_of_fixes; i++){
		EEPROM_API_ReadBuffer(eEepromAT24C256A1, (i-1)*33, (uint8_t*)latitude, 8);
		EEPROM_API_ReadBuffer(eEepromAT24C256A1, (i-1)*33+8, (uint8_t*)longitude, 8);
		EEPROM_API_ReadBuffer(eEepromAT24C256A1, (i-1)*33+16, (uint8_t*)time_of_fix, 17);
		snprintf((char*)log_tx, 100, "Time: %s, lattitude: %s, longitude: %s\n", time_of_fix, latitude, longitude);
		UART_API_SendString(eUartDebug, (char*)log_tx, 100);
	}

	return true;
}
bool Misc_Command_OptionsSave(char *params){
	uint8_t options[100]={0};
	uint16_t length = ParseToSymbol(params, options, ',');
	uint8_t length_copy = length;
	if (length == 255) return false;  
	if (length == 1) length = options[0] - '0'; 
	else if (length == 2) length = 10*(options[0] - '0') + (options[1] - '0');
	else if (length == 3) length = 100*(options[0] - '0') + 10*(options[1] - '0') + options[2] - '0';
	if (length > eOptionsLast) return false; 
	switch ((eMiscOptions_t) (length)){
		case eOptionsMode:{
			if (params[length_copy+1] == '0'){
			    pet_tracker_mode = PET_TRACKER_MODE_NORMAL;
			}
			else if ((params[length_copy+1] == '1')) pet_tracker_mode = PET_TRACKER_MODE_POWER_SAVE;
			if ((params[length_copy+1] == '0') || (params[length_copy+1] == '1'))
			    EEPROM_API_SendByte(EEPROM_OPTIONS, EE_PET_TRACKER_MODE_8B, pet_tracker_mode);

			break;
		}
		case eOptionsNormalCount:{
		    length = ParseToSymbol(params+length_copy+1, options, '\n');
		    if (length == 1) length = options[0] - '0';
		    else if (length == 2) length = 10*(options[0] - '0') + (options[1] - '0');
		    else if (length == 3) length = 100*(options[0] - '0') + 10*(options[1] - '0') + options[2] - '0';
            else if (length == 4) length = 1000*(options[0] - '0') + 100*(options[1] - '0') + 10*(options[2] - '0') + (options[3] - '0');
		    if ((length>1400) || (length<1)) return false;
		    else {
		        normal_mode_time = length*2;
		        EEPROM_API_SendBuffer(EEPROM_OPTIONS, EE_PET_TRACKER_FIX_FREQ_NM_16B, (uint8_t*)&normal_mode_time, 2);
		    }
		    break;
		}
		case eOptionsPowerSaveCount:{
		    length = ParseToSymbol(params+length_copy+1, options, '\n');
		    if (length == 1) length = options[0] - '0';
		    else if (length == 2) length = 10*(options[0] - '0') + (options[1] - '0');
		    else if (length == 3) length = 100*(options[0] - '0') + 10*(options[1] - '0') + options[2] - '0';
		    else if (length == 4) length = 1000*(options[0] - '0') + 100*(options[1] - '0') + 10*(options[2] - '0') + (options[3] - '0');
		    if ((length>1400) || (length<5)) return false;
		    else {
		        power_save_mode_time = length*2;
		        EEPROM_API_SendBuffer(EEPROM_OPTIONS, EE_PET_TRACKER_FIX_FREQ_PSM_16B, (uint8_t*)&power_save_mode_time, 2);
		    }
		    break;
		}
		default:
		break;
	}



	return true;
}


//UTILITY 

uint8_t ParseToSymbol(char *text, char *parsed, char symbol) {
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
