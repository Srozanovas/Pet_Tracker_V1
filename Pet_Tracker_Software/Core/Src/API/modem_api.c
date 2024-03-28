#include "modem_api.h"
#include "uart_api.h"




bool Modem_API_SendCommand(char * params){
    UART_API_SendString(eUartModem, params, 250);
    return true;
}

bool Modem_API_GNSS_Power(char *params){
    return true;
}
