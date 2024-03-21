#include "power_api.h"
#include "main.h"
#include "stdlib.h"
#include "gpio_driver.h"


uint8_t POWER_API_ParseLevel(char* params) { 
    uint8_t power = 2;  
    for (int i = 0; i<50; i++){ 
        if (*(params+i) == '1') { 
            power = 1;
            break; 
        } else if (*(params+i) == '0'){
            power = 0; 
            break; 
        } else if ((*(params+i) == 0) || (*(params+i) == '\n')){ 
            power = 2; 
            break; 
        } else { 
            power = 2; 
        }
    }
    return power;
}

bool POWER_API_PinControl(eGpioPin_t gpio_pin, uint8_t power){ 
    switch (gpio_pin) {
        case eGpioPinA12LEDsOn: {
            if (power == 0) { //turn off
                GPIO_Driver_WritePin(gpio_pin, ePinHigh); 
            } else if (power == 1){ //turn on 
                GPIO_Driver_WritePin(gpio_pin, ePinLow);
            } else { // undefined 
                return false;
            }
            break;
        }
        case eGpioPinA6GSMPower: 
        case eGpioPinB0Power4V: {
            if (power == 0) { //turn off
                GPIO_Driver_WritePin(gpio_pin, ePinLow); 
            } else if (power == 1){ //turn on 
                GPIO_Driver_WritePin(gpio_pin, ePinHigh);
            } else { // undefined 
                return false;
            }
            break;
        }
        default: return false;
    }
    return true;
} 




bool POWER_API_LEDPower(char* params){
    return POWER_API_PinControl(eGpioPinA12LEDsOn, POWER_API_ParseLevel(params));
}



bool POWER_API_4VPower(char* params){
    return POWER_API_PinControl(eGpioPinB0Power4V, POWER_API_ParseLevel(params));
} 
bool POWER_API_GNSSPower(char *params){
    return true;
} 
bool POWER_API_ModemPower(char *params){
    return POWER_API_PinControl(eGpioPinA6GSMPower, POWER_API_ParseLevel(params));
}

