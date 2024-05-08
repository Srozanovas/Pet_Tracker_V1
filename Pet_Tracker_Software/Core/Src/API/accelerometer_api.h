#ifndef __ACCELEROMETER_API__H__
#define __ACCELEROMETER_API__H__

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "main.h"
#include "i2c_driver.h"
#include "cmsis_os.h"

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
#define MPU6050_ADRESS 0xD0
#define WHAT_AM_I_REGISTER 0x75
#define PWR_MGMT1_REGISTER 0x6B
#define SMPLRT_DIV_REGISTER 0x19
#define GYRO_CONFIG_REGISTER 0x1B
#define ACCEL_CONFIG_REGISTER 0x1C
#define ENABLE_INTERUPT_REGISTER 0x38
#define INTERUPT_CONFIG_REGISTER 0x37
#define CONFIG_REGISTER 0x1A

#define INTERUPT_STATUS_REGISTER 0x3A

#define ACCE_X_H_REGISTER 0x3B
#define ACCE_Y_H_REGISTER 0x3D
#define ACCE_Z_H_REGISTER 0x3F
typedef struct sAccDesc_t {
    eI2C_t i2c_id;
    uint8_t adress;
} sAccDesc_t;

typedef struct sAccelerometerAxisDesc_t {
    float kgain;
    float meas_error;
    float current_estimate_error;
    float previous_estimate_error;
    float current_estimate;
    float previous_estimate;
    float error;
    short raw_value;
    float measurement;
    float qvalue;
    float rvalue;
} sAccelerometerAxisDesc_t;

typedef enum eAccAxies_t {
    eAccXaxis,
        eAccYaxis,
        eAccZaxis
} eAccAxies_t;

typedef enum eAcc_t {
    eAccFirst = 0,
        eAccMPU6050 = eAccFirst,
        eAccLast
} eAcc_t;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
bool ACC_API_Init (eAcc_t acc_id);
bool ACC_API_RaiseFlag (uint8_t flag);
bool ACC_API_ReadRegister (eAcc_t acc_id, uint8_t register_adress, uint8_t *rx_buffer, uint8_t rx_size);
bool ACC_API_WriteRegister (eAcc_t acc_id, uint8_t register_adress, uint8_t tx_data);
bool ACC_API_Resume();

#endif /* __CMD_LIST__H__ */
