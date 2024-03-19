#ifndef __CMD_API__H__
#define __CMD_API__H__

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "stdio.h"
//#include "string_utility.h"
#include "uart_api.h"



typedef enum eCommandModules_t { 
    eCommandModulesFirst = 0, 
    eCommandModulesModem = eCommandModulesFirst, 
    eCommandModulesLED, 
    eCommandModulesPCUART, 
    eCommandModulesPower, 
    eCommandModulesAcce, 
    eCommandModulesEEPROM,
    eCommandModulesLast,
    eCommandModulesERROR
} eCommandModules_t;





typedef struct sCommandParameters_t {
   eCommandModules_t module; 
   uint8_t command; 
   char *params; 
} sCommandParameters_t;


bool CMD_API_ThreadInit (void);

#endif /* __CMD_APP__H__ */
