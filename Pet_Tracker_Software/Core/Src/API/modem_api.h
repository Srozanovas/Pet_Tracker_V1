#ifndef MODEM_API_H
#define MODEM_API_H

#include "main.h"
#include "stdbool.h"



typedef enum eModemSendWait { 
    eModemSendWaitNo = 0, 
    eModemSendWaitFlagCustom, 
    eModemSendWaitFlagForever, 
    eModemSendWaitCustom,
    eModemSendWaitLast
} eModemSendWait;

//THREAD
bool Modem_API_Init(); 

//COMMAND FUNCTIONS 
bool Modem_API_SendCommand(char * params); 
bool Modem_API_GNSS_Power(char *params);
bool Modem_API_SendSMS(char *params);
bool Modem_API_DownloadEPO(char *params);
bool Modem_API_CheckEpo(char *params);
bool Modem_API_ReadSMS(char *params);
bool Modem_API_DeleteSMS(char *params);
bool Modem_API_GetLocation(char *params);
bool Modem_API_SendLocation(char *params);

//UTILITY 
uint32_t Modem_API_SendWait(char * params, eModemSendWait modem_wait, ...);


#endif
