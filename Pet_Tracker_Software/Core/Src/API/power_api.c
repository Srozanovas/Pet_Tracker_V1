#include "power_api.h"
#include "gpio_driver.h"
#include "cmsis_os2.h"


uint16_t power_stats; 

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
            if (power == 0){ //turn off                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              if (power == 0) { //turn off
                if(GPIO_Driver_WritePin(gpio_pin, ePinHigh) == true)
                    power_stats &= ~LED_ON;  
                
            } else if (power == 1){ //turn on 
                if (GPIO_Driver_WritePin(gpio_pin, ePinLow) == true) { 
                    power_stats |= LED_ON;
                }
            }else{   // undefined 
                return false;
            }
            break;
        }
        case eGpioPinB0Power4V: {
            if (power == 0) { //turn off
                if (GPIO_Driver_WritePin(gpio_pin, ePinLow) == true){
                    power_stats &= ~V4_ON;  
                }
            } else if (power == 1){ //turn on 
                if (GPIO_Driver_WritePin(gpio_pin, ePinHigh) == true){
                    power_stats |= V4_ON;
                }
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
    uint8_t power = POWER_API_ParseLevel(params);
    if(power == 1 ){ 
        if ((power_stats & V4_ON) == 0){  //4V power not on 
            if (POWER_API_PinControl(eGpioPinB0Power4V, 1) == false) return false;
        }
        if (GPIO_Driver_WritePin(eGpioPinA6GSMPower, ePinLow) == false) return false; 
        osDelay(2000); 
        if (GPIO_Driver_WritePin(eGpioPinA6GSMPower, ePinHigh) == false) return false; 
    }
    return true;
}

