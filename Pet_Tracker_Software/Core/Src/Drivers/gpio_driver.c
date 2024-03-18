
////////////////////////////////////////INCLUDES/////////////////////////////////////////////

#include "gpio_driver.h"
#include "main.h"
//#include  "debug_api.h"

/*______________________________________DEFINES, CONSTANTS, VARIABLES_____________________________________*/
#define RTC_H_CLOCK_ENABLED (1<<3)
#define RTC_A_CLOCK_ENABLED (1<<0)
#define RTC_B_CLOCK_ENABLED (1<<1)
#define RTC_C_CLOCK_ENABLED (1<<2)
uint8_t rtc_clock_enabled = 0; 


typedef struct sGpioDesc_t {
    GPIO_TypeDef *port;
    uint32_t pin;
    uint32_t mode;
    uint32_t speed;
    uint32_t pull;
    uint32_t alternate; 
    bool interupt_enable; 
    IRQn_Type interupt;
} sGpioDesc_t;


const static sGpioDesc_t gpio_desc_lut[] = {
    //GPIOA______________________________________________________________________________________________________________________________________________________________________________________________________________________________________
    [eGpioPinA1AcceInt]     = {.port = GPIOA, .pin = GPIO_PIN_1,    .mode = GPIO_MODE_IT_FALLING,   .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_PULLUP, .alternate = 0,                .interupt_enable = true,    .interupt=EXTI1_IRQn    },
    [eGpioPinA2UART2TX]     = {.port = GPIOA, .pin = GPIO_PIN_2,    .mode = GPIO_MODE_AF_PP,        .speed = GPIO_SPEED_FREQ_VERY_HIGH, .pull = GPIO_NOPULL, .alternate = GPIO_AF7_USART2,  .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinA3UART2RX]     = {.port = GPIOA, .pin = GPIO_PIN_3,    .mode = GPIO_MODE_AF_PP,        .speed = GPIO_SPEED_FREQ_VERY_HIGH, .pull = GPIO_NOPULL, .alternate = GPIO_AF7_USART2,  .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinA4]            = {.port = GPIOA, .pin = GPIO_PIN_4,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinA5]            = {.port = GPIOA, .pin = GPIO_PIN_5,    .mode = GPIO_MODE_EVT_RISING,   .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinA6GSMPower]    = {.port = GPIOA, .pin = GPIO_PIN_6,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinA7]            = {.port = GPIOA, .pin = GPIO_PIN_7,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinA9I2C1SCL]     = {.port = GPIOA, .pin = GPIO_PIN_9,    .mode = GPIO_MODE_AF_OD,        .speed = GPIO_SPEED_FREQ_VERY_HIGH, .pull = GPIO_NOPULL, .alternate = GPIO_AF4_I2C1,    .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinA10I2C1SDA]    = {.port = GPIOA, .pin = GPIO_PIN_10,   .mode = GPIO_MODE_AF_OD,        .speed = GPIO_SPEED_FREQ_VERY_HIGH, .pull = GPIO_NOPULL, .alternate = GPIO_AF4_I2C1,    .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinA11]           = {.port = GPIOA, .pin = GPIO_PIN_11,   .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinA12LEDsOn]     = {.port = GPIOA, .pin = GPIO_PIN_12,   .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    //GPIOB______________________________________________________________________________________________________________________________________________________________________________________________________________________________________
    [eGpioPinB0Power4V]     = {.port = GPIOB, .pin = GPIO_PIN_0,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinB1]            = {.port = GPIOB, .pin = GPIO_PIN_1,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinB2]            = {.port = GPIOB, .pin = GPIO_PIN_2,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinB6UART1TX]     = {.port = GPIOB, .pin = GPIO_PIN_6,    .mode = GPIO_MODE_AF_PP,        .speed = GPIO_SPEED_FREQ_VERY_HIGH, .pull = GPIO_NOPULL, .alternate = GPIO_AF7_USART1,  .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinB7UART1RX]     = {.port = GPIOB, .pin = GPIO_PIN_7,    .mode = GPIO_MODE_AF_PP,        .speed = GPIO_SPEED_FREQ_VERY_HIGH, .pull = GPIO_NOPULL, .alternate = GPIO_AF7_USART1,  .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinB10I2C2SCL]    = {.port = GPIOB, .pin = GPIO_PIN_10,   .mode = GPIO_MODE_AF_OD,        .speed = GPIO_SPEED_FREQ_VERY_HIGH, .pull = GPIO_NOPULL, .alternate = GPIO_AF4_I2C2,    .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinB11I2C2SDA]    = {.port = GPIOB, .pin = GPIO_PIN_11,   .mode = GPIO_MODE_AF_OD,        .speed = GPIO_SPEED_FREQ_VERY_HIGH, .pull = GPIO_NOPULL, .alternate = GPIO_AF4_I2C2,    .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinB12]           = {.port = GPIOB, .pin = GPIO_PIN_12,   .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinB13]           = {.port = GPIOB, .pin = GPIO_PIN_13,   .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinB14]           = {.port = GPIOB, .pin = GPIO_PIN_14,   .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinB15]           = {.port = GPIOB, .pin = GPIO_PIN_15,   .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    //GPIOC______________________________________________________________________________________________________________________________________________________________________________________________________________________________________
    [eGpioPinC4]            = {.port = GPIOC, .pin = GPIO_PIN_4,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_PULLUP, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinC6]            = {.port = GPIOC, .pin = GPIO_PIN_6,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinC7]            = {.port = GPIOC, .pin = GPIO_PIN_7,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinC8]            = {.port = GPIOC, .pin = GPIO_PIN_8,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    [eGpioPinC9]            = {.port = GPIOC, .pin = GPIO_PIN_9,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false,   .interupt=0xFF          },
    };
/*______________________________________DEFINES, CONSTANTS, VARIABLES_____________________________________*/



bool GPIO_Driver_Init (eGpioPin_t gpio_pin, eGpioPinState_t init_state) {  //GPIO PIN INITIALIZATION

    //UNDEFINED PIN 

    if (gpio_pin >= eGpioPinLast) {
        return false;
    }
    if (init_state >= ePinNaN){
        return false; 
    }

    GPIO_InitTypeDef gpio_init_struct = {0};

    //CLOCKS ENABLE 

    if ((rtc_clock_enabled&RTC_H_CLOCK_ENABLED)==0){
        __HAL_RCC_GPIOH_CLK_ENABLE();
        rtc_clock_enabled|=RTC_H_CLOCK_ENABLED;      
    }
    switch ((int)(gpio_desc_lut[gpio_pin].port)){
        case (int)(GPIOA):{
            if ((rtc_clock_enabled&RTC_A_CLOCK_ENABLED)==0){
                 __HAL_RCC_GPIOA_CLK_ENABLE();
                rtc_clock_enabled|=RTC_A_CLOCK_ENABLED;      
            } 
            break;
        }
        case (int)(GPIOB):{
            if ((rtc_clock_enabled&RTC_B_CLOCK_ENABLED)==0){
                 __HAL_RCC_GPIOB_CLK_ENABLE();
                rtc_clock_enabled|=RTC_B_CLOCK_ENABLED;      
            } 
            break;
        }
        case (int)(GPIOC):{
            if ((rtc_clock_enabled&RTC_C_CLOCK_ENABLED)==0){
                 __HAL_RCC_GPIOC_CLK_ENABLE();
                rtc_clock_enabled|=RTC_C_CLOCK_ENABLED;      
            } 
            break;
        }
        default: return false;                 
    }

    //GPIO STRUCT FORMATION
    
    HAL_GPIO_WritePin(gpio_desc_lut[gpio_pin].port, gpio_desc_lut[gpio_pin].pin, GPIO_PIN_RESET);
    gpio_init_struct.Pin = gpio_desc_lut[gpio_pin].pin;
    gpio_init_struct.Mode = gpio_desc_lut[gpio_pin].mode;
    gpio_init_struct.Speed = gpio_desc_lut[gpio_pin].speed;
    gpio_init_struct.Pull = gpio_desc_lut[gpio_pin].pull;
    gpio_init_struct.Alternate = gpio_desc_lut[gpio_pin].alternate;
    
    HAL_GPIO_Init(gpio_desc_lut[gpio_pin].port, &gpio_init_struct);

    //INTERUPT ENABLE 
    if (gpio_desc_lut[gpio_pin].interupt_enable == true){
        HAL_NVIC_SetPriority(gpio_desc_lut[gpio_pin].interupt, 5, 0);
        HAL_NVIC_EnableIRQ(gpio_desc_lut[gpio_pin].interupt);
    }

    if (init_state == ePinHigh) HAL_GPIO_WritePin(gpio_desc_lut[gpio_pin].port, gpio_desc_lut[gpio_pin].pin, GPIO_PIN_SET);
    
    return true;
}

bool GPIO_Driver_TogglePin (eGpioPin_t gpio_pin) {  //TOGGLE_GPIO_PIN
    if (gpio_pin >= eGpioPinLast) {
        return false;
    }
    HAL_GPIO_TogglePin(gpio_desc_lut[gpio_pin].port, gpio_desc_lut[gpio_pin].pin);
    return true;
}

bool GPIO_Driver_WritePin (eGpioPin_t gpio_pin, eGpioPinState_t pin_state) {    //SET GPIO PIN
    if (gpio_pin >= eGpioPinLast) {
        return false;
    }
    switch (pin_state) {
        case ePinLow:
            HAL_GPIO_WritePin(gpio_desc_lut[gpio_pin].port, gpio_desc_lut[gpio_pin].pin, GPIO_PIN_SET);
            break;
        case ePinHigh:
            HAL_GPIO_WritePin(gpio_desc_lut[gpio_pin].port, gpio_desc_lut[gpio_pin].pin, GPIO_PIN_RESET);
            break;
        default:
            return false;
    }
    return true;
}

bool GPIO_Driver_ReadPin (eGpioPin_t gpio_pin, eGpioPinState_t *pin_status) {   //READ GPIO PIN
    if (pin_status == NULL) {
        return false;
    }
    if (gpio_pin >= eGpioPinLast) {
        *pin_status = ePinNaN;
        return false;
    }
    GPIO_PinState state=HAL_GPIO_ReadPin(gpio_desc_lut[gpio_pin].port,  gpio_desc_lut[gpio_pin].pin); 
    switch (state) { 
        case GPIO_PIN_RESET:    *pin_status = ePinLow; break; 
        case GPIO_PIN_SET:      *pin_status = ePinHigh; break; 
        default:                *pin_status = ePinNaN; return false;
    }
    return true;
}

bool GPIO_Driver_InitAll (void) {   //INIT ALL PINS
    eGpioAllPin_t all_good = eGpioAllPinsOK;
    for (eGpioPin_t pin = eGpioPinFirst; pin < eGpioPinLast; pin++) {
        if (GPIO_Driver_Init(pin, ePinLow) == false) {
            all_good = eGpioAllPinsNotOk;
        }
    }
    if (all_good == eGpioAllPinsNotOk) {
        return false;
    }
    return true;
}


/*____________________INTERUPT HANDLERS_______________________________________*/

void EXTI1_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}
