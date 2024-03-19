#ifndef __CLI_FUNCTION_LIST__H__
#define __CLI_FUNCTION_LIST__H__


#include "stdbool.h"
#include "main.h"



typedef struct sCommandFunctions_t {
    char *name;
    bool (*function_pointer) (char * func_params);
} sCommandFunctions_t;


//FUNCTION ENUMS 


typedef enum eModemCommands_t { 
    eModemCommandsFirst = 0, 
    eModemCommandsPower = eModemCommandsFirst, 
    eModemCommandsGNSSPower, 
    eModemCommandsLast
} eModemCommands_t;





extern const sCommandFunctions_t modem_command_function_lut[eModemCommandsLast];

uint8_t Command_Functions_Get_Function_Number (eModemCommands_t module, char * command_name); 


#endif /* __CMD_LIST__H__ */
