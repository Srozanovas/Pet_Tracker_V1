#ifndef POWER_API_H
#define POWER_API_H

#include "stdbool.h"
#include "stdlib.h"
#include "main.h"

#define LED_ON  (1<<0)
#define V4_ON   (1<<1)
#define GSM_ON  (1<<2)
#define GNSS_ON (1<<3) 


extern uint16_t power_stats; 




bool POWER_API_LEDPower(char* params);
bool POWER_API_4VPower(char* params); 
bool POWER_API_GNSSPower(char *params); 
bool POWER_API_ModemPower(char *params);



#endif 