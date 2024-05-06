#ifndef __CMD_API__H__
#define __CMD_API__H__


#include "stdio.h"

#include "uart_api.h"



typedef enum eCommandModules_t { 
    eCommandModulesFirst = 0, 
    eCommandModulesModem = eCommandModulesFirst, 
    eCommandModulesLED, 
    eCommandModulesPCUART, 
    eCommandModulesPower, 
    eCommandModulesAcce, 
    eCommandModulesEEPROM,
    eCommandModulesMisc,
    eCommandModulesLast,
    eCommandModulesERROR
} eCommandModules_t;





typedef struct sCommandParameters_t {
   eCommandModules_t module; 
   uint8_t command; 
   char *params; 
} sCommandParameters_t;


bool CMD_API_ThreadInit (void);
bool CMD_API_GetFromQueue (sCommandParameters_t **command, uint32_t wait_time);
bool CMD_API_AddCommandToQueue(sCommandParameters_t *comand);
bool CMD_API_PuttoQueue (sCommandParameters_t *command);

#endif /* __CMD_APP__H__ */
