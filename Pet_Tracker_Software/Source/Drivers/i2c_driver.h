#ifndef SOURCE_DRIVER_I2C_DRIVER_H_
#define SOURCE_DRIVER_I2C_DRIVER_H_


#include "main.h"
#include "stdbool.h"
#include "gpio_driver.h"

typedef enum eI2C_t {
        eI2CFirst = 0,
        eI2CAcce = eI2CFirst,
        eI2CEeprom,
        eI2CLast
} eI2C_t;



bool I2C_Driver_Init(eI2C_t i2c_name);
bool I2C_Low_Level_Init(eI2C_t i2c_name);
bool I2C_Driver_Send(eI2C_t I2C, uint8_t device_adress, uint16_t memory_adress, uint8_t mem_size, uint8_t *tx_data, uint8_t size); //I2C SEND DATA
bool I2C_Driver_Read(eI2C_t I2C, uint8_t device_adress, uint16_t memory_adress, uint8_t mem_size, uint8_t *rx_data, uint8_t size); //I2C READ DATA







#endif /* SOURCE_DRIVER_I2C_DRIVER_H_ */
