// /**********************************************************************************************************************
//  * Includes
//  *********************************************************************************************************************/
// #include "eeprom_api.h"
// #include "cmsis_os.h"
// /**********************************************************************************************************************
//  * Private definitions and macros
//  *********************************************************************************************************************/

// /**********************************************************************************************************************
//  * Private typedef
//  *********************************************************************************************************************/

// /**********************************************************************************************************************
//  * Private constants
//  *********************************************************************************************************************/

// /**********************************************************************************************************************
//  * Private variables
//  *********************************************************************************************************************/
// static sEepromDesc_t eeprom_desc_lut[] = {
//     [eEepromAT24C256A0] = {
//         .adress = 0xA0,
//         .current_page = 13,
//         .page_offset = 0,
//         .num_of_measures = 0,
//         .last_page = 0,
//         .last_page_offset = 0,
//         .i2c_id = eI2CEeprom,

//     },
//     [eEepromAT24C256A2] = {
//         .adress = 0xA2,
//         .current_page = 0,
//         .page_offset = 0,
//         .i2c_id = eI2CEeprom,
//         .num_of_measures = 0,
//         .last_page = 0,
//         .last_page_offset = 0,
//     },

// };

// /**********************************************************************************************************************
//  * Exported variables and references
//  *********************************************************************************************************************/

// /**********************************************************************************************************************
//  * Prototypes of private functions
//  *********************************************************************************************************************/

// /**********************************************************************************************************************
//  * Definitions of private functions
//  *********************************************************************************************************************/

// /**********************************************************************************************************************
//  * Definitions of exported functions
//  *********************************************************************************************************************/

// bool EEPROM_API_Init (void) {
//     if (I2C_Driver_Init(eeprom_desc_lut[eEepromAT24C256A0].i2c_id) != true) {
//         return false;
//     }
//     return true;
// }

// bool EEPROM_API_SendByte (eEeprom_t eeprom_id, uint8_t *adress, uint8_t byte) {
//     uint8_t tx_buf[3];
//     tx_buf[0] = adress[0];
//     tx_buf[1] = adress[1];
//     tx_buf[2] = byte;
//     I2C_Driver_SendByte(eeprom_desc_lut[eeprom_id].i2c_id, eeprom_desc_lut[eeprom_id].adress, tx_buf, 3, LL_I2C_MODE_AUTOEND);

//     return true;
// }

// bool EEPROM_API_ReadByte (eEeprom_t eeprom_id, uint8_t *adress, uint8_t *byte) {
//     uint8_t tx_buf[2];
//     tx_buf[0] = adress[0];
//     tx_buf[1] = adress[1];
//     I2C_Driver_SendByte(eeprom_desc_lut[eeprom_id].i2c_id, eeprom_desc_lut[eeprom_id].adress, tx_buf, 2, LL_I2C_MODE_RELOAD);
//     I2C_Driver_ReadByte(eeprom_desc_lut[eeprom_id].i2c_id, eeprom_desc_lut[eeprom_id].adress, byte, 1);
//     return true;
// }

// bool EEPROM_API_SendBufferSequently () {

//     return true;
// }

// bool EEPROM_API_ReadLast () {

//     return true;
// }

// bool EEPROM_API_SendBuffer (eEeprom_t eeprom_id, uint8_t *adress, uint8_t *tx_buffer, uint8_t buffer_size) {
//     uint8_t tx_buf[256];
//     tx_buf[0] = adress[0];
//     tx_buf[1] = adress[1];
//     for (int i = 2; i < buffer_size + 2; i++) {
//         tx_buf[i] = tx_buffer[i - 2];
//     }
//     I2C_Driver_SendByte(eeprom_desc_lut[eeprom_id].i2c_id, eeprom_desc_lut[eeprom_id].adress, tx_buf, buffer_size + 2, LL_I2C_MODE_AUTOEND);

//     return true;
// }

// bool EEPROM_API_ReadBuffer (eEeprom_t eeprom_id, uint8_t *adress, uint8_t *rx_buffer, uint8_t buffer_size) {
//     uint8_t tx_buf[2];
//     tx_buf[0] = adress[0];
//     tx_buf[1] = adress[1];
//     I2C_Driver_SendByte(eeprom_desc_lut[eeprom_id].i2c_id, eeprom_desc_lut[eeprom_id].adress, tx_buf, 2, LL_I2C_MODE_RELOAD);
//     I2C_Driver_ReadByte(eeprom_desc_lut[eeprom_id].i2c_id, eeprom_desc_lut[eeprom_id].adress, rx_buffer, buffer_size);
//     return true;
// }

// bool EEPROM_API_ReadCurrentPage (eEeprom_t eeprom_id, uint8_t *rx_buffer, uint8_t buffer_size) {
//     uint16_t full_adress = 0;
//     if (eeprom_desc_lut[eEepromAT24C256A0].page_offset == 30) {
//         full_adress = eeprom_desc_lut[eEepromAT24C256A0].current_page << 6 | (eeprom_desc_lut[eEepromAT24C256A0].page_offset-30);
//     } else
//     {
//         full_adress = (eeprom_desc_lut[eEepromAT24C256A0].current_page-1) << 6 | (eeprom_desc_lut[eEepromAT24C256A0].page_offset+30);

//     }
//     uint8_t adress[2];
//     adress[0] = full_adress >> 8;
//     adress[1] = full_adress;
//     EEPROM_API_ReadBuffer(eeprom_id, adress, rx_buffer, buffer_size);
//     return true;
// }

// bool EEPROM_API_ReadPage (eEeprom_t eeprom_id, uint8_t page, uint8_t offset, uint8_t *rx_buffer, uint8_t buffer_size) {
//     uint16_t full_adress = 0;
//     full_adress = page << 6 | offset;
//     uint8_t adress[2];
//     adress[0] = full_adress >> 8;
//     adress[1] = full_adress;
//     EEPROM_API_ReadBuffer(eeprom_id, adress, rx_buffer, buffer_size);
//     return true;

// }

// bool EEPROM_API_SequentalBufferTransmit (eEeprom_t eeprom_id, uint8_t *tx_buffer, uint8_t buffer_size) {
//     uint16_t full_adress = 0;
//     full_adress = eeprom_desc_lut[eEepromAT24C256A0].current_page << 6 | eeprom_desc_lut[eEepromAT24C256A0].page_offset;
//     uint8_t adress[2];
//     adress[0] = full_adress >> 8;
//     adress[1] = full_adress;
//     EEPROM_API_SendBuffer(eeprom_id, adress, tx_buffer, buffer_size);
//     eeprom_desc_lut[eeprom_id].num_of_measures++;
//     switch ((uint8_t) eeprom_id) {
//         case eEepromAT24C256A0: {
//             if (eeprom_desc_lut[eeprom_id].num_of_measures % 2 == 0) {
//                 eeprom_desc_lut[eeprom_id].page_offset = 0;
//                 eeprom_desc_lut[eeprom_id].current_page++;
//             }
//             else {
//                 if (eeprom_desc_lut[eeprom_id].num_of_measures > 200) {
//                     eeprom_desc_lut[eeprom_id].num_of_measures = 0;
//                     eeprom_desc_lut[eeprom_id].current_page = 1;
//                     eeprom_desc_lut[eeprom_id].page_offset = 0;
//                 }
//                 eeprom_desc_lut[eeprom_id].page_offset = 30;
//             }

//         }
//     }
//     return true;

// }

