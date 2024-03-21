#ifndef __CLI_FUNCTION_LIST__H__
#define __CLI_FUNCTION_LIST__H__


#include "stdbool.h"
#include "main.h"



typedef struct sCommandFunctions_t {
    char name[30];
    bool (*function_pointer) (char * func_params);
} sCommandFunctions_t;


//FUNCTION ENUMS 

//all modem functions 
typedef enum eModemCommands_t { 
    eModemCommandsFirst = 0, 
    eModemCommandsPower = eModemCommandsFirst, 
    eModemCommandsGNSSPower, 
    eModemCommandsLast
} eModemCommands_t;

//all led functions
typedef enum eLEDCommands_t { 
    eLEDCommandsFirst = 0, 

    eLEDCommandsLast
} eLEDCommands_t;

//all pc uart functions
typedef enum ePCUARTCommands_t { 
    ePCUARTCommandsFirst = 0, 

    ePCUARTCommandsLast
} ePCUARTCommands_t;

//all power commands
typedef enum ePowerCommands_t { 
    ePowerCommandsFirst = 0, 
    ePowerCommandsLEDPower = ePowerCommandsFirst, 
    ePowerCommands4VPower, 
    ePowerCommandsGNSSPower, 
    ePowerCommandsModemPower,
    ePowerCommandsLast
} ePowerCommands_t;

//all acce commands
typedef enum eAcceCommands_t { 
    eAcceCommandsFirst = 0, 

    eAcceCommandsLast
} eAcceCommands_t;

//all eeprom commands
typedef enum eEEPROMCommands_t { 
    eEEPROMCommandsFirst = 0, 

    eEEPROMCommandsLast
} eEEPROMCommands_t;




extern const sCommandFunctions_t modem_command_function_lut[eModemCommandsLast];
extern const sCommandFunctions_t led_command_function_lut[eLEDCommandsLast];
extern const sCommandFunctions_t pcuart_command_function_lut[ePCUARTCommandsLast];
extern const sCommandFunctions_t power_command_function_lut[ePowerCommandsLast];
extern const sCommandFunctions_t acce_command_function_lut[eAcceCommandsLast];
extern const sCommandFunctions_t eeprom_command_function_lut[eEEPROMCommandsLast];



#endif /* __CMD_LIST__H__ */
