

#include "i2c_driver.h"
#include "stdbool.h"
#include "../Drivers/gpio_driver.h"



typedef struct sI2CDesc_t {
    I2C_TypeDef *instance;
    uint32_t    timing;
    uint32_t    analog_filter;
    uint32_t    digital_filter;
    uint32_t    own_adress1;
    uint32_t    own_adress2;
    uint32_t    own_adress2_masks; 
    uint32_t    adressing_mode;
    uint32_t    dual_adress_mode;
    uint32_t    general_call_mode; 
    uint32_t    no_stretch_mode;
    eGpioPin_t  scl_pin;
    eGpioPin_t  sda_pin;
} sI2CDesc_t;

typedef struct sI2CDescDynamic_t { 
    I2C_HandleTypeDef i2c_handle; 
} sI2CDescDynamic_t; 



const static sI2CDesc_t i2c_desc_lut[] = {
           [eI2CAcce]   = { .instance =      I2C2,
                            .timing   =      0x00000E14,
                            .analog_filter   = I2C_ANALOGFILTER_ENABLE,
                            .digital_filter  = 0,
                            .own_adress1     = 0,
                            .own_adress2     = 0, 
                            .own_adress2_masks   = I2C_OA2_NOMASK, 
                            .adressing_mode      = I2C_ADDRESSINGMODE_7BIT, 
                            .dual_adress_mode    = I2C_DUALADDRESS_DISABLE, 
                            .general_call_mode   = I2C_GENERALCALL_DISABLE, 
                            .no_stretch_mode     = I2C_NOSTRETCH_DISABLE,
                            .sda_pin = eGpioPinB11I2C2SDA,
                            .scl_pin = eGpioPinB10I2C2SCL
           },
           [eI2CEeprom] = { .instance =      I2C1,
                            .timing   =      0x00000E14,
                            .analog_filter   = I2C_ANALOGFILTER_ENABLE,
                            .digital_filter  = 0,
                            .own_adress1     = 0,
                            .own_adress2     = 0, 
                            .own_adress2_masks   = I2C_OA2_NOMASK, 
                            .adressing_mode      = I2C_ADDRESSINGMODE_7BIT, 
                            .dual_adress_mode    = I2C_DUALADDRESS_DISABLE, 
                            .general_call_mode   = I2C_GENERALCALL_DISABLE, 
                            .no_stretch_mode     = I2C_NOSTRETCH_DISABLE,
                            .scl_pin = eGpioPinA9I2C1SCL,		
	                        .sda_pin = eGpioPinA10I2C1SDA,
           }
};
static sI2CDescDynamic_t i2c_desc_lut_dynamic[] = {
        [eI2CAcce] =    {.i2c_handle = {0}}, 
        [eI2CEeprom] =  {.i2c_handle = {0}}, 
};


bool I2C_Driver_Init(eI2C_t i2c_name) {
    if (i2c_name >= eI2CLast){
        return false;
    }
    if (I2C_Low_Level_Init(i2c_name) == false){ 
        return false; 
    }
    i2c_desc_lut_dynamic[i2c_name].i2c_handle.Instance = i2c_desc_lut[i2c_name].instance; 
    i2c_desc_lut_dynamic[i2c_name].i2c_handle.Init.Timing = i2c_desc_lut[i2c_name].timing; 
    i2c_desc_lut_dynamic[i2c_name].i2c_handle.Init.OwnAddress1 = i2c_desc_lut[i2c_name].own_adress1; 
    i2c_desc_lut_dynamic[i2c_name].i2c_handle.Init.DualAddressMode = i2c_desc_lut[i2c_name].dual_adress_mode; 
    i2c_desc_lut_dynamic[i2c_name].i2c_handle.Init.OwnAddress2 = i2c_desc_lut[i2c_name].own_adress2;
    i2c_desc_lut_dynamic[i2c_name].i2c_handle.Init.OwnAddress2Masks = i2c_desc_lut[i2c_name].own_adress2_masks;  
    i2c_desc_lut_dynamic[i2c_name].i2c_handle.Init.GeneralCallMode = i2c_desc_lut[i2c_name].general_call_mode;
    i2c_desc_lut_dynamic[i2c_name].i2c_handle.Init.NoStretchMode = i2c_desc_lut[i2c_name].no_stretch_mode;
    if (
        (HAL_I2C_Init(&i2c_desc_lut_dynamic[i2c_name].i2c_handle) != HAL_OK)
        ||(HAL_I2CEx_ConfigAnalogFilter(&i2c_desc_lut_dynamic[i2c_name].i2c_handle, i2c_desc_lut[i2c_name].analog_filter) != HAL_OK)
        ||(HAL_I2CEx_ConfigDigitalFilter(&i2c_desc_lut_dynamic[i2c_name].i2c_handle, i2c_desc_lut[i2c_name].digital_filter) != HAL_OK)
    )  Error_Handler();

    return true; 


}


bool I2C_Low_Level_Init(eI2C_t i2c_name){
    if (GPIO_Driver_Init(i2c_desc_lut[i2c_name].sda_pin) != true){
        return false;
    }
    if (GPIO_Driver_Init(i2c_desc_lut[i2c_name].scl_pin) != true) {
        return false;
    }
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    if(i2c_name == eI2CEeprom) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
        PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
            Error_Handler();
        }
        __HAL_RCC_I2C1_CLK_ENABLE();
        return true; 
    }
    if(i2c_name == eI2CAcce) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
        PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
            Error_Handler();
        }
        __HAL_RCC_I2C2_CLK_ENABLE();
        return true; 
    }
    return false; 
} 








// bool I2C_Driver_SendByte(eI2C_t i2c_name, uint8_t slave_address,  uint8_t* tx_buffer, uint8_t transfer_size, uint32_t end_mode) {
//     LL_I2C_HandleTransfer(i2c_desc_lut[i2c_name].I2C, slave_address, LL_I2C_OWNADDRESS1_7BIT, transfer_size, end_mode, LL_I2C_GENERATE_START_WRITE);
//     uint32_t counter=0;
//     while (!LL_I2C_IsActiveFlag_TXE(i2c_desc_lut[i2c_name].I2C)){
//         counter++;
//         if (counter==10000){
//         return false;
//         }
//     }
//     for (uint8_t byte=0; byte<transfer_size; byte++){
//         LL_I2C_TransmitData8(i2c_desc_lut[i2c_name].I2C, tx_buffer[byte]);
//         while (!LL_I2C_IsActiveFlag_TXE(i2c_desc_lut[i2c_name].I2C)){
//             counter++;
//             if (counter==10000){
//             return false;
//             }
//         }
//     }
//     return true;
// }

// bool I2C_Driver_ReadByte(eI2C_t i2c_name, uint8_t slave_address, uint8_t* rx_buffer, uint8_t read_size) {
//       uint32_t counter=0;
//       LL_I2C_HandleTransfer(i2c_desc_lut[i2c_name].I2C, slave_address, LL_I2C_OWNADDRESS1_7BIT, read_size, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
//       for (uint8_t byte=0; byte<read_size; byte++){
//           while (!LL_I2C_IsActiveFlag_RXNE(i2c_desc_lut[i2c_name].I2C)){
//               counter++;
//               if (counter==10000){
//                   return false;
//               }
//           }
//                 rx_buffer[byte]=LL_I2C_ReceiveData8(i2c_desc_lut[i2c_name].I2C);
//                 counter=0;
//       }


//       return true;
// }






//  uint8_t t[10]="LABAS";
//  while (HAL_I2C_Mem_Write(&hi2c1, 0xA0, 0x00, I2C_MEMADD_SIZE_16BIT , t, 5, 5000)!=HAL_OK){}
//  uint8_t k[10]={0};
//  while (HAL_I2C_Mem_Read( &hi2c1, 0xA0, 0x00, I2C_MEMADD_SIZE_16BIT, k, 10, 1000) != HAL_OK){}

