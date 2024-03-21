#ifndef POWER_API_H
#define POWER_API_H

#include "stdbool.h"


bool POWER_API_LEDPower(char* params);
bool POWER_API_4VPower(char* params); 
bool POWER_API_GNSSPower(char *params); 
bool POWER_API_ModemPower(char *params);



#endif 