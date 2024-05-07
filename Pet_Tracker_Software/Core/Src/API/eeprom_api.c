
#include "eeprom_api.h"
#include "cmsis_os.h"

static sEepromDesc_t eeprom_desc_lut[] = {
    [eEepromAT24C256A0] = {
        .adress = 0xA0,     //1010 000x    x-write read
        .current_adress = 0, //last 
    },
    [eEepromAT24C256A1] = {
        .adress = 0xA2,     //1010 001x    x-write read
        .current_adress = 0
    },

};


bool EEPROM_API_Init (void) {
    if (I2C_Driver_Init(eI2CEeprom) != true) {
        return false;
    }
    return true;
}

bool EEPROM_API_SendByte (eEeprom_t eeprom_id, uint16_t adress, uint8_t byte) {
    I2C_Driver_Send(eI2CEeprom, eeprom_desc_lut[eeprom_id].adress, adress, 2, &byte, 1);
    return true;
}

bool EEPROM_API_ReadByte (eEeprom_t eeprom_id, uint16_t adress, uint8_t *byte) {
    I2C_Driver_Read(eI2CEeprom, eeprom_desc_lut[eeprom_id].adress, adress, 2, byte, 1);
    return true;
}

bool EEPROM_API_SendBuffer(eEeprom_t eeprom_id, uint16_t adress, uint8_t *byte, uint8_t size){ 
    for (uint8_t i = 0; i<size; i++){ 
        EEPROM_API_SendByte(eeprom_id, adress+i, (byte[i]));
        osDelay(5);
    }
    return true;
} 

bool EEPROM_API_EraseSector(eEeprom_t eeprom_id, uint16_t start_adress, uint16_t finish_adress){ 

    for (int i = start_adress; i<=finish_adress; i++){ 
        EEPROM_API_SendByte(eeprom_id, i, 0xFF);
    }
    return true;
}

bool EEPROM_API_ReadBuffer(eEeprom_t eeprom_id, uint16_t adress, uint8_t *byte, uint8_t size){
    
    for (uint8_t i = 0; i<size; i++){ 
        EEPROM_API_ReadByte(eeprom_id, adress+i, (byte+i));
    }
    return true;
}

