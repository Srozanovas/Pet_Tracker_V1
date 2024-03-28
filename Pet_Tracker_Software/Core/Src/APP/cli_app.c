//#include "debug_api.h"
#include "cmd_api.h"
#include "cli_app.h"
#include "cli_function_list.h"


const osThreadAttr_t cli_app_thread_atr = {
    .name = "cli_app_thread",
    .stack_size = 1024,
    .priority = osPriorityNormal
};


osThreadId_t cli_app_thread_id = NULL;
sCommandParameters_t *cli_app_command_struct;


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
    	if(CMD_API_GetFromQueue(&cli_app_command_struct, osWaitForever) == true){
            switch (cli_app_command_struct->module) {
                case eCommandModulesModem:{
                    modem_command_function_lut[cli_app_command_struct->command].function_pointer(cli_app_command_struct->params);
                    break; 
                }
                case eCommandModulesLED: {
                    led_command_function_lut[cli_app_command_struct->command].function_pointer(cli_app_command_struct->params);
                    break; 
                }
                case eCommandModulesPCUART:{
                    pcuart_command_function_lut[cli_app_command_struct->command].function_pointer(cli_app_command_struct->params);
                    break;
                }
                case eCommandModulesPower:{
                    power_command_function_lut[cli_app_command_struct->command].function_pointer(cli_app_command_struct->params);
                    break;
                } 
                case eCommandModulesAcce:{
                    acce_command_function_lut[cli_app_command_struct->command].function_pointer(cli_app_command_struct->params);
                    break;
                } 
                case eCommandModulesEEPROM:{
                    eeprom_command_function_lut[cli_app_command_struct->command].function_pointer(cli_app_command_struct->params);
                    break;
                }
                case eCommandModulesERROR:
                default : break;
            }

        free(cli_app_command_struct->params);
        free(cli_app_command_struct); 

        }   
    }
    osThreadTerminate(cli_app_thread_id);
}
/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
