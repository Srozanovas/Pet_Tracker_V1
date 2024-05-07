#ifndef EEPROM_API_H_
#define EEPROM_API_H_

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "main.h"
#include "i2c_driver.h"
#include "stdbool.h"
/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
typedef enum eEeprom_t {
    eEepromFirst = 0,
    eEepromAT24C256A0 = eEepromFirst,
    eEepromAT24C256A1,
    eEepromLast
}eEeprom_t;


typedef struct sEepromDesc_t {
       uint8_t adress;
       uint16_t current_adress;
}sEepromDesc_t;
/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
**********************************************************************************************************************/
bool EEPROM_API_Init();
bool EEPROM_API_SendByte (eEeprom_t eeprom_id, uint16_t adress, uint8_t byte);
bool EEPROM_API_ReadByte (eEeprom_t eeprom_id, uint16_t adress, uint8_t *byte);
bool EEPROM_API_SendBuffer(eEeprom_t eeprom_id, uint16_t adress, uint8_t *byte, uint8_t size);
bool EEPROM_API_ReadBuffer(eEeprom_t eeprom_id, uint16_t adress, uint8_t *byte, uint8_t size);
bool EEPROM_API_EraseSector(eEeprom_t eeprom_id, uint16_t start_adress, uint16_t finish_adress);

//EEPROM MAP ----------------------------------------------------------------------------- 0000 - 7FFF

#define EE_LAST_LOCATION_ADRESS_16B  
#define EE_PET_TRACKER_OPTIONS_16B          0x0000  //2bytes
#define EE_PET_TRACKER_APN_50X8B            0x0002  //50bytes
#define EE_PET_TRACKER_PHONEBOOK10x20x8B    0x0034  //200bytes
#define EE_LAST_FIX 0xFFF0

#define EE_LAST 0x7FFF




















#endif /* SOURCE_API_EEPROM_API_H_ */
