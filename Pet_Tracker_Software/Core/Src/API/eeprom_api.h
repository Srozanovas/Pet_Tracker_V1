// #ifndef SOURCE_API_EEPROM_API_H_
// #define SOURCE_API_EEPROM_API_H_

// /**********************************************************************************************************************
//  * Includes
//  *********************************************************************************************************************/
// #include "main.h"
// #include "i2c_driver.h"
// #include "stdbool.h"
// /**********************************************************************************************************************
//  * Exported definitions and macros
//  *********************************************************************************************************************/

// /**********************************************************************************************************************
//  * Exported types
//  *********************************************************************************************************************/
// typedef enum eEeprom_t {
//     eEepromFirst = 0,
//     eEepromAT24C256A0 = eEepromFirst,
//     eEepromAT24C256A2,
//     eEepromLast
// }eEeprom_t;


// typedef struct sEepromDesc_t {
//        eI2C_t i2c_id;
//        uint8_t adress;
//        uint16_t current_page;
//        uint8_t page_offset;
//        uint16_t num_of_measures;
//        uint8_t last_page;
//        uint8_t last_page_offset;
// }sEepromDesc_t;
// /**********************************************************************************************************************
//  * Exported variables
//  *********************************************************************************************************************/

// /**********************************************************************************************************************
//  * Prototypes of exported functions
// **********************************************************************************************************************/
// bool EEPROM_API_Init();
// bool EEPROM_API_SendByte (eEeprom_t eeprom_id, uint8_t *adress, uint8_t byte);
// bool EEPROM_API_ReadByte (eEeprom_t eeprom_id, uint8_t *adress, uint8_t *byte);
// bool EEPROM_API_ReadCurrentPage (eEeprom_t eeprom_id, uint8_t *rx_buffer, uint8_t buffer_size);
// bool EEPROM_API_SequentalBufferTransmit(eEeprom_t eeprom_id, uint8_t *tx_buffer, uint8_t buffer_size);

// bool EEPROM_API_ReadPage(eEeprom_t eeprom_id, uint8_t page, uint8_t offset, uint8_t *rx_buffer, uint8_t buffer_size);
// bool EEPROM_API_ReadLast();


// bool EEPROM_API_SendBuffer(eEeprom_t eeprom_id, uint8_t *adress, uint8_t *tx_buffer, uint8_t buffer_size);

// bool EEPROM_API_ReadBuffer(eEeprom_t eeprom_id, uint8_t *adress, uint8_t *rx_buffer, uint8_t buffer_size);


// #endif /* SOURCE_API_EEPROM_API_H_ */
