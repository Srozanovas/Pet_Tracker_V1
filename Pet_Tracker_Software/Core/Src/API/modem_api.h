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



bool Modem_API_SendCommand(char * params); 
bool Modem_API_GNSS_Power(char *params);
bool Modem_API_Init(); 
bool Modem_API_SendWait(char * params, eModemSendWait modem_wait, ...);
bool Modem_API_SendSMS(char *params);




#endif