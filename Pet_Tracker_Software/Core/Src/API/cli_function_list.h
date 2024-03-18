#ifndef __CLI_FUNCTION_LIST__H__
#define __CLI_FUNCTION_LIST__H__

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "stdbool.h"
#include "main.h"

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
typedef struct sCommandResponse_t {
    char *message_command;
    uint8_t command_length;
    char *message_args;
    uint8_t args_length;
    char *response_buffer;
    uint8_t response_buffer_length;
} sCommandResponse_t;

typedef struct sFunctionName_t {
    char *name;
    uint8_t name_length;
    bool (*function_pointer) (sCommandResponse_t*);
} sFunctionName_t;

typedef enum eFunctionListCLI_t {
        eFunctionListCLIFirst = 0,
		eFunctionListCLIModemSendCommand,
		eFunctionListCLIModemSendSMS,
		eFunctionListCLIModemReadSMS,
		eFunctionListCLIModemConfig,
        eFunctionListCLILast
} eFunctionListCLI_t;



extern const sFunctionName_t function_lut_cli[eFunctionListCLILast];

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/


bool CMD_API_ModemSendCommand(sCommandResponse_t *command_response);
bool CMD_API_ModemSendSMS(sCommandResponse_t *command_response);
bool CMD_API_ModemReadSMS(sCommandResponse_t *command_response);
bool CMD_API_ModemConfig(sCommandResponse_t *command_response);


#endif /* __CMD_LIST__H__ */
