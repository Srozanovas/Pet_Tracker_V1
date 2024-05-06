#include "power_api.h"
#include "gpio_driver.h"
#include "cmsis_os2.h"

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
                if(GPIO_Driver_WritePin(gpio_pin, ePinHigh) != true) return false; 
                
            } else if (power == 1){ //turn on 
                if (GPIO_Driver_WritePin(gpio_pin, ePinLow) != true)  return false;
            }else{   // undefined 
                return false;
            }
            break;
        }
        case eGpioPinB0Power4V: {
            if (power == 0) { //turn off
                if (GPIO_Driver_WritePin(gpio_pin, ePinLow) != true) return false; 
            } else if (power == 1){ //turn on 
                if (GPIO_Driver_WritePin(gpio_pin, ePinHigh) != true) return false; 
                
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
	uint8_t power = POWER_API_ParseLevel(params);
	if ((gpio_init_status & (1<<eGpioPinB0Power4V)) == 0){ //if not initialized
        GPIO_Driver_Init(eGpioPinB0Power4V, ePinLow);
        if ((gpio_init_status & (1<<eGpioPinB0Power4V)) == 0) return false; //didn't initialize 
    }
    return POWER_API_PinControl(eGpioPinB0Power4V, POWER_API_ParseLevel(params));
} 


bool POWER_API_ModemPower(char *params){
    uint8_t power = POWER_API_ParseLevel(params);
    if(power == 1){ 
    	if ((pet_tracker_status & MODEM_POWER_ON) != 0) return true; //already powered
        //CHECK IF NECESSERY PINS ARE INITIALIZED AND ON: 
        if ((gpio_pin_level_status & (1<<eGpioPinB0Power4V)) == 0) { 
            if ((gpio_init_status & (1<<eGpioPinB0Power4V)) == 0) { 
                GPIO_Driver_Init(eGpioPinB0Power4V, ePinHigh); 
                if ((gpio_init_status & (1<<eGpioPinB0Power4V)) == 0) return false;
                if ((gpio_pin_level_status & (1<<eGpioPinB0Power4V)) == 0) return false;
           } else {
                GPIO_Driver_WritePin(eGpioPinB0Power4V, ePinHigh); 
                if ((gpio_pin_level_status & (1<<eGpioPinB0Power4V)) == 0) return false;
           }
        }
        if ((gpio_pin_level_status & (1<<eGpioPinA6GSMPower)) == 0){ 
            if ((gpio_init_status & (1<<eGpioPinA6GSMPower)) == 0) { 
                GPIO_Driver_Init(eGpioPinA6GSMPower, ePinHigh); 
                if ((gpio_init_status & (1<<eGpioPinA6GSMPower)) == 0) return false;
                if ((gpio_pin_level_status & (1<<eGpioPinA6GSMPower)) == 0) return false;
            } else {
                GPIO_Driver_ReadPin(eGpioPinA6GSMPower, ePinHigh); 
                if ((gpio_pin_level_status & (1<<eGpioPinA6GSMPower)) == 0) return false;
            }
        }

        //TURNING ON MODEM: 
        if (GPIO_Driver_WritePin(eGpioPinA6GSMPower, ePinLow) == false) return false; 
        osDelay(1000); 
        if (GPIO_Driver_WritePin(eGpioPinA6GSMPower, ePinHigh) == false) return false; 
        
        pet_tracker_status |= MODEM_POWER_ON; // add power on flag


    } else {  
    	if ((pet_tracker_status & MODEM_POWER_ON) == 0) return true; //already off
        if ((gpio_pin_level_status & (1<<eGpioPinA6GSMPower)) != 0) { 
            if (GPIO_Driver_WritePin(eGpioPinA6GSMPower, ePinLow) == false) return false; 
            osDelay(1000); 
            if (GPIO_Driver_WritePin(eGpioPinA6GSMPower, ePinHigh) == false) return false; 
        }
        pet_tracker_status &= ~MODEM_POWER_ON; //clear power on flag
        pet_tracker_status &= ~MODEM_INITIALISED;
    }

    return true;
}

