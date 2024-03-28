#ifndef MODEM_API_H
#define MODEM_API_H

#include "main.h"
#include "stdbool.h"


bool Modem_API_SendCommand(char * params); 
bool Modem_API_GNSS_Power(char *params);







#endif