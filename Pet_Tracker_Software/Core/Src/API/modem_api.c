#include "modem_api.h"
#include "uart_api.h"
#include "string.h"
#include "stdio.h"
#include  "stdlib.h"



bool Modem_API_SendCommand(char * params){
    UART_API_SendString(eUartModem, params, 250);
    return true;
}

bool Modem_API_GNSS_Power(char *params){
    uint8_t i=0;
    while (1) { 
        if (params[i]==0 || params[i]=='\n') return false; //no data
        if (params[i]-48 == 1 || params[i]-48 == 0) i = params[i] - 48;  //extract 1 or 0 
    }
    char modem_tx[20]={0};
    strncpy(modem_tx, "AT+CGNSPWR=%d\n", i);
    return true;
}


