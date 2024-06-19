/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "accelerometer_api.h"
#include "i2c_driver.h"
#include "gpio_driver.h"
#include "cmsis_os.h"
#include "math.h"
#include "uart_api.h"
#include "modem_api.h"
#include "string.h"
#include "stdio.h"
#include "main.h"
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/


#define PWR_UP_DEFAULT 0x00
#define PWR_UP_DEFAULT1 0x00
#define GYRO_CONFIG_DEFAULT 0x00
#define ACCEL_CONFIG_DEFAULT 0x00
#define SMPLRT_DIV_DEFAULT 0x0A
#define ENABLE_INTERUPT_DATA_READY 0x01
#define INTERUPT_CONFIG 0x20
#define CONFIG_DEFAULT 0x00




static sAccDesc_t acc_desc_lut[] = {
    [eAccMPU6050] = {
        .adress = MPU6050_ADRESS,
        .i2c_id = eI2CAcce,
    }
};

const osThreadAttr_t acce_api_thread_atr = {
    .name = "acce_api_thread",
    .stack_size = 1024,
    .priority = osPriorityNormal};

osThreadId_t  acce_thread_id;



sAccelerometerAxisDesc_t axies_desc_lut[] = {
    [eAccXaxis] = {
        .kgain = 0,
        .meas_error = 0.5,
        .current_estimate_error = 0,
        .previous_estimate_error = 0.83,
        .current_estimate = 0,
        .previous_estimate = 0,
        .measurement = 0,
        .raw_value = 0,
        .qvalue = 0.55,
        .rvalue = 0.83

    },
    [eAccYaxis] = {
        .kgain = 0,
        .meas_error = 0.5,
        .current_estimate_error = 0,
        .previous_estimate_error = 0.83,
        .current_estimate = 0,
        .previous_estimate = 0,
        .measurement = 0,
        .raw_value = 0,
        .qvalue = 0.55,
        .rvalue = 0.83


    },
    [eAccZaxis] = {
        .kgain = 0,
        .meas_error = 0.5,
        .current_estimate_error = 0,
        .previous_estimate_error = 0.83,
        .current_estimate = 0,
        .previous_estimate = 0,
        .measurement = 0,
        .raw_value = 0,
        .qvalue = 0.55,
        .rvalue = 0.83


    },

};






osEventFlagsId_t acce_flags = NULL;

// //Accelerometer data
static uint8_t accelerometer_data[6];
float Raw;

//acceleration buffer
float acceleration_buf = 0;
uint16_t measures = 0;

uint8_t data;
// uint16_t temp_acc;
// float final;
 
void ACC_API_Thread (void *arguments);
bool ACC_API_WaitFlag ();
bool ACC_API_ReadAllAcceleration ();
bool ACC_API_KalmanFiltering();
bool ACC_API_Suspend();


bool ACC_API_Init (eAcc_t acc_id) {
    acce_thread_id = osThreadNew(ACC_API_Thread, NULL, &acce_api_thread_atr);
    if (acce_thread_id == NULL) {
       return false;
    }


    acce_flags = osEventFlagsNew(NULL);
    if (acce_flags == NULL) {
       return false;
    }

    if (acc_id >= eAccLast) {
        return false;
    }

    if (I2C_Driver_Init(acc_desc_lut[acc_id].i2c_id) != true) {
        return false;
    }
    if (GPIO_Driver_Init(eGpioPinA1AcceInt, ePinLow) != true) {
        return false;
    }
    GPIO_Driver_AcceFlagFunc(&ACC_API_RaiseFlag);
    ACC_API_ReadRegister(eAccMPU6050, WHAT_AM_I_REGISTER, &data, 1);

    if (data != 0x68) {
        return false;
    }
    ACC_API_WriteRegister(eAccMPU6050, INTERUPT_CONFIG_REGISTER, INTERUPT_CONFIG);
    ACC_API_WriteRegister(eAccMPU6050, ENABLE_INTERUPT_REGISTER, ENABLE_INTERUPT_DATA_READY);
    ACC_API_WriteRegister(eAccMPU6050, PWR_MGMT1_REGISTER, PWR_UP_DEFAULT);
    ACC_API_WriteRegister(eAccMPU6050, SMPLRT_DIV_REGISTER, SMPLRT_DIV_DEFAULT);
    ACC_API_WriteRegister(eAccMPU6050, GYRO_CONFIG_REGISTER, GYRO_CONFIG_DEFAULT);
    ACC_API_WriteRegister(eAccMPU6050, ACCEL_CONFIG_REGISTER, ACCEL_CONFIG_DEFAULT);

    ACC_API_ReadRegister(eAccMPU6050, INTERUPT_STATUS_REGISTER, &data, 1);


    return true;
}

bool ACC_API_KalmanFiltering(){ 
    static uint16_t config_flag = 0;
    static float Xaxis_conv_sum = 0;
    static float Yaxis_conv_sum = 0;
    static float Zaxis_conv_sum = 0;

    //parse axis meassurements 
    axies_desc_lut[eAccXaxis].measurement = (axies_desc_lut[eAccXaxis].raw_value * 1.0 / 16384) - axies_desc_lut[eAccXaxis].error;
    axies_desc_lut[eAccYaxis].measurement = (axies_desc_lut[eAccYaxis].raw_value * 1.0 / 16384) - axies_desc_lut[eAccYaxis].error;
    axies_desc_lut[eAccZaxis].measurement = (axies_desc_lut[eAccZaxis].raw_value * 1.0 / 16384) - axies_desc_lut[eAccZaxis].error;

    //calculate full acceleration 
    Raw=axies_desc_lut[eAccXaxis].measurement*axies_desc_lut[eAccXaxis].measurement+axies_desc_lut[eAccYaxis].measurement*axies_desc_lut[eAccYaxis].measurement+axies_desc_lut[eAccZaxis].measurement*axies_desc_lut[eAccZaxis].measurement;

    //calibrating ()
    if (config_flag < 1000) {
        Xaxis_conv_sum += axies_desc_lut[eAccXaxis].measurement;
        Yaxis_conv_sum += axies_desc_lut[eAccYaxis].measurement;
        Zaxis_conv_sum += axies_desc_lut[eAccZaxis].measurement;
        config_flag++;
    }
    //calculating offset
    if (config_flag >= 1000) {
        axies_desc_lut[eAccXaxis].error = Xaxis_conv_sum / 1000;
        axies_desc_lut[eAccYaxis].error = Yaxis_conv_sum / 1000 - 1;
        axies_desc_lut[eAccZaxis].error = Zaxis_conv_sum / 1000;
        acce_status |= ACCE_CALIBRATED;
    }
    //kalman filter
    if ((acce_status & ACCE_CALIBRATED) != 0) {
        //prediction stage


        axies_desc_lut[eAccXaxis].current_estimate_error =  axies_desc_lut[eAccXaxis].previous_estimate_error + axies_desc_lut[eAccXaxis].qvalue;
        axies_desc_lut[eAccYaxis].current_estimate_error =  axies_desc_lut[eAccYaxis].previous_estimate_error + axies_desc_lut[eAccYaxis].qvalue;
        axies_desc_lut[eAccZaxis].current_estimate_error =  axies_desc_lut[eAccZaxis].previous_estimate_error + axies_desc_lut[eAccZaxis].qvalue;

        axies_desc_lut[eAccXaxis].current_estimate = axies_desc_lut[eAccXaxis].previous_estimate;
        axies_desc_lut[eAccYaxis].current_estimate = axies_desc_lut[eAccYaxis].previous_estimate;
        axies_desc_lut[eAccZaxis].current_estimate = axies_desc_lut[eAccZaxis].previous_estimate;


        //correction stage


        axies_desc_lut[eAccXaxis].kgain = axies_desc_lut[eAccXaxis].current_estimate_error/(axies_desc_lut[eAccXaxis].current_estimate_error+axies_desc_lut[eAccXaxis].rvalue);
        axies_desc_lut[eAccYaxis].kgain = axies_desc_lut[eAccYaxis].current_estimate_error/(axies_desc_lut[eAccYaxis].current_estimate_error+axies_desc_lut[eAccYaxis].rvalue);
        axies_desc_lut[eAccZaxis].kgain = axies_desc_lut[eAccZaxis].current_estimate_error/(axies_desc_lut[eAccZaxis].current_estimate_error+axies_desc_lut[eAccZaxis].rvalue);

        axies_desc_lut[eAccXaxis].current_estimate_error -=  axies_desc_lut[eAccXaxis].current_estimate_error*axies_desc_lut[eAccXaxis].kgain;
        axies_desc_lut[eAccYaxis].current_estimate_error -=  axies_desc_lut[eAccYaxis].current_estimate_error*axies_desc_lut[eAccYaxis].kgain;
        axies_desc_lut[eAccZaxis].current_estimate_error -=  axies_desc_lut[eAccZaxis].current_estimate_error*axies_desc_lut[eAccYaxis].kgain;

        axies_desc_lut[eAccXaxis].current_estimate += axies_desc_lut[eAccXaxis].kgain * (axies_desc_lut[eAccXaxis].measurement - axies_desc_lut[eAccXaxis].current_estimate);
        axies_desc_lut[eAccYaxis].current_estimate += axies_desc_lut[eAccYaxis].kgain * (axies_desc_lut[eAccYaxis].measurement - axies_desc_lut[eAccYaxis].current_estimate);
        axies_desc_lut[eAccZaxis].current_estimate += axies_desc_lut[eAccZaxis].kgain * (axies_desc_lut[eAccZaxis].measurement - axies_desc_lut[eAccZaxis].current_estimate);

        //update

        axies_desc_lut[eAccXaxis].previous_estimate = axies_desc_lut[eAccXaxis].current_estimate;
        axies_desc_lut[eAccYaxis].previous_estimate = axies_desc_lut[eAccYaxis].current_estimate;
        axies_desc_lut[eAccZaxis].previous_estimate = axies_desc_lut[eAccZaxis].current_estimate;

        axies_desc_lut[eAccXaxis].previous_estimate_error = axies_desc_lut[eAccXaxis].current_estimate_error;
        axies_desc_lut[eAccYaxis].previous_estimate_error = axies_desc_lut[eAccYaxis].current_estimate_error;
        axies_desc_lut[eAccZaxis].previous_estimate_error = axies_desc_lut[eAccZaxis].current_estimate_error;


        //full acceleration

        acceleration = axies_desc_lut[eAccXaxis].current_estimate * axies_desc_lut[eAccXaxis].current_estimate +
                        axies_desc_lut[eAccYaxis].current_estimate * axies_desc_lut[eAccYaxis].current_estimate +
                        axies_desc_lut[eAccZaxis].current_estimate * axies_desc_lut[eAccZaxis].current_estimate;
        acceleration = sqrt(acceleration) - 1;
        }
        
       return true;

}

// float Raw;


 void ACC_API_Thread (void *argument) {
     while (true) {
        //blocking if using interupt
        static uint8_t first_start = 0; 

        if ((acce_status & ACCE_ENABLE) == 0) {
            if (first_start != 1) first_start = 1; 
            average_acceleration = acceleration_buf / measures;
            measures = 0; 
            acceleration_buf = 0;
            ACC_API_Suspend();
        }

        if ((acce_options & ACCE_USE_INTERUPT) != 0){
            if (first_start == 1){ 
                first_start = 0; 
                ACC_API_ReadRegister(eAccMPU6050, INTERUPT_STATUS_REGISTER, &data, 1);
            }
            ACC_API_WaitFlag(); 
        }


        ACC_API_ReadAllAcceleration();

        ACC_API_KalmanFiltering(); 
        (acceleration<0) ? (acceleration_buf-=acceleration) : (acceleration_buf+=acceleration);
        measures++; 




        if ((acce_options & ACCE_USE_INTERUPT) != 0){
            ACC_API_ReadRegister(eAccMPU6050, INTERUPT_STATUS_REGISTER, &data, 1);
        } else osDelay(2);
    }
    osThreadTerminate(acce_thread_id);
}

bool ACC_API_WriteRegister (eAcc_t acc_id, uint8_t register_adress, uint8_t tx_data) {
    I2C_Driver_Send(acc_desc_lut[acc_id].i2c_id, acc_desc_lut[acc_id].adress, register_adress, 1, &tx_data, 1);
    return true;
}

bool ACC_API_ReadRegister (eAcc_t acc_id, uint8_t register_adress, uint8_t *rx_buffer, uint8_t rx_size) {
    I2C_Driver_Read(acc_desc_lut[acc_id].i2c_id, acc_desc_lut[acc_id].adress, register_adress, 1, rx_buffer, rx_size);
    return true;
}



bool ACC_API_RaiseFlag (uint8_t flag) {
    osEventFlagsSet(acce_flags, ACCE_DATA_READY);
    return true; 
}

bool ACC_API_WaitFlag () {
     uint32_t flag = osEventFlagsWait(acce_flags, ACCE_ENABLE, osFlagsWaitAny, osWaitForever);
     if (flag == ACCE_ENABLE) {
        return true;
    }
    return false;
}

bool ACC_API_ReadAllAcceleration () {
    ACC_API_ReadRegister(eAccMPU6050, ACCE_X_H_REGISTER, accelerometer_data, 6);
    axies_desc_lut[eAccXaxis].raw_value = accelerometer_data[0] << 8 | accelerometer_data[1];
    axies_desc_lut[eAccYaxis].raw_value = accelerometer_data[2] << 8 | accelerometer_data[3];
    axies_desc_lut[eAccZaxis].raw_value = accelerometer_data[4] << 8 | accelerometer_data[5];
    return true;
}



bool ACC_API_Resume(){

    if (osThreadGetState(acce_thread_id)==osThreadReady || osThreadGetState(acce_thread_id) == osThreadRunning){
        return true;
    }
    osThreadResume(acce_thread_id);
    return true;
}


bool ACC_API_Suspend() {
	if (osThreadGetState(acce_thread_id) == osThreadBlocked){
		return true;
	}
	osThreadSuspend(acce_thread_id);
	return true;
}

