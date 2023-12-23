/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "gpio_driver.h"
#include "main.h"
//#include  "debug_api.h"


/**********************************************************************************************************************
 * Definitions, macros, variables 
 *********************************************************************************************************************/
//DEBUG_MODULE(GPIO_DRIVER);

#define RTC_H_CLOCK_ENABLED (1<<3)
#define RTC_A_CLOCK_ENABLED (1<<0)
#define RTC_B_CLOCK_ENABLED (1<<1)
#define RTC_C_CLOCK_ENABLED (1<<2)


uint8_t rtc_clock_enabled = 0; 

typedef enum eGpioInterupt_t {
    eGpioInteruptFirst = 0,
    eGpioInteruptAccelerometer = eGpioInteruptFirst,
    eGpioInteruptLast
}eGpioInterupt_t;

typedef enum eGpioExternal_t {
        eGpioExternal,
        eGpioNotExternal,
}eGpioExternal_t ;

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

// typedef struct sGPioExternalDesc_t {
//     uint32_t line031;
//     uint32_t line3263;
//     FunctionalState command;
//     uint8_t mode;
//     uint8_t trigger;
//     uint32_t interupt_port;
//     uint32_t interupt_pin;
//     IRQn_Type interupt_type;
//     bool (*function_pointer)(void);
// }sGPioExternalDesc_t;


const static sGpioDesc_t gpio_desc_lut[] = {
    //GPIOA
    [eGpioPinA1]            = {.port = GPIOA, .pin = GPIO_PIN_1,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinA2UART2TX]     = {.port = GPIOA, .pin = GPIO_PIN_2,    .mode = GPIO_MODE_AF_PP,        .speed = GPIO_SPEED_FREQ_VERY_HIGH, .pull = GPIO_NOPULL, .alternate = GPIO_AF3_USART2,  .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinA3UART2RX]     = {.port = GPIOA, .pin = GPIO_PIN_3,    .mode = GPIO_MODE_AF_PP,        .speed = GPIO_SPEED_FREQ_VERY_HIGH, .pull = GPIO_NOPULL, .alternate = GPIO_AF3_USART2,  .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinA4AcceInt]     = {.port = GPIOA, .pin = GPIO_PIN_4,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinA5GSMEnable]   = {.port = GPIOA, .pin = GPIO_PIN_5,    .mode = GPIO_MODE_EVT_RISING,   .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinA6]            = {.port = GPIOA, .pin = GPIO_PIN_6,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinA7]            = {.port = GPIOA, .pin = GPIO_PIN_7,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinA9I2C1SCL]     = {.port = GPIOA, .pin = GPIO_PIN_9,    .mode = GPIO_MODE_AF_OD,        .speed = GPIO_SPEED_FREQ_VERY_HIGH, .pull = GPIO_NOPULL, .alternate = GPIO_AF4_I2C1,    .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinA10I2C1SDA]    = {.port = GPIOA, .pin = GPIO_PIN_10,   .mode = GPIO_MODE_AF_OD,        .speed = GPIO_SPEED_FREQ_VERY_HIGH, .pull = GPIO_NOPULL, .alternate = GPIO_AF4_I2C1,    .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinA11]           = {.port = GPIOA, .pin = GPIO_PIN_11,   .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinA12]           = {.port = GPIOA, .pin = GPIO_PIN_12,   .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinA15]           = {.port = GPIOA, .pin = GPIO_PIN_15,   .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    //GPIOB
    [eGpioPinB0]            = {.port = GPIOB, .pin = GPIO_PIN_0,    .mode = GPIO_MODE_EVT_RISING,   .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = true,  .interupt=EXTI0_IRQn  },
    [eGpioPinB1]            = {.port = GPIOB, .pin = GPIO_PIN_1,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinB2]            = {.port = GPIOB, .pin = GPIO_PIN_2,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinB6UART1TX]     = {.port = GPIOB, .pin = GPIO_PIN_6,    .mode = GPIO_MODE_AF_PP,        .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = GPIO_AF7_USART1,  .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinB7UART1RX]     = {.port = GPIOB, .pin = GPIO_PIN_7,    .mode = GPIO_MODE_AF_PP,        .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = GPIO_AF7_USART1,  .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinB10I2C2SCL]    = {.port = GPIOB, .pin = GPIO_PIN_10,   .mode = GPIO_MODE_AF_OD,        .speed = GPIO_SPEED_FREQ_VERY_HIGH, .pull = GPIO_NOPULL, .alternate = GPIO_AF4_I2C2,    .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinB11I2C2SDA]    = {.port = GPIOB, .pin = GPIO_PIN_11,   .mode = GPIO_MODE_AF_OD,        .speed = GPIO_SPEED_FREQ_VERY_HIGH, .pull = GPIO_NOPULL, .alternate = GPIO_AF4_I2C2,    .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinB12]           = {.port = GPIOB, .pin = GPIO_PIN_12,   .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinB13]           = {.port = GPIOB, .pin = GPIO_PIN_13,   .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinB14]           = {.port = GPIOB, .pin = GPIO_PIN_14,   .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinB15]           = {.port = GPIOB, .pin = GPIO_PIN_15,   .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    //GPIOC
    [eGpioPinC4]            = {.port = GPIOC, .pin = GPIO_PIN_4,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinC6]            = {.port = GPIOC, .pin = GPIO_PIN_6,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinC7]            = {.port = GPIOC, .pin = GPIO_PIN_7,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinC8]            = {.port = GPIOC, .pin = GPIO_PIN_8,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    [eGpioPinC9]            = {.port = GPIOC, .pin = GPIO_PIN_9,    .mode = GPIO_MODE_OUTPUT_PP,    .speed = GPIO_SPEED_FREQ_LOW,       .pull = GPIO_NOPULL, .alternate = 0,                .interupt_enable = false, .interupt=0xFF        },
    };


// // static sGPioExternalDesc_t  external_desc_lut[] = {
// //         [eGpioInteruptAccelerometer] = {.line031 = LL_EXTI_LINE_9, .line3263 = LL_EXTI_LINE_NONE, .command = ENABLE, .mode = LL_EXTI_MODE_IT, .trigger =  LL_EXTI_TRIGGER_RISING, .interupt_port = LL_SYSCFG_EXTI_PORTC, .interupt_pin = LL_SYSCFG_EXTI_LINE9, .interupt_type = EXTI9_5_IRQn, .function_pointer=NULL }
// // };



bool GPIO_Driver_Init (eGpioPin_t gpio_pin, ...) {

    //UNDEFINED PIN 

    if (gpio_pin >= eGpioPinLast) {
        return false;
    }

    GPIO_InitTypeDef gpio_init_struct = {0};

    //CLOCKS ENABLE 

    if (rtc_clock_enabled&RTC_H_CLOCK_ENABLED==0){
        __HAL_RCC_GPIOH_CLK_ENABLE();
        rtc_clock_enabled&=RTC_H_CLOCK_ENABLED;      
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
        
        __NVIC_SetPriorityGrouping(0);
        __NVIC_SetPriority(EXTI0_IRQn, 30);
        __NVIC_EnableIRQ(EXTI0_IRQn);
       // HAL_NVIC_EnableIRQ(EXTI0_IRQn);
       // HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    }

    return true;
}

bool GPIO_Driver_TogglePin (eGpioPin_t gpio_pin) {
    if (gpio_pin >= eGpioPinLast) {
        return false;
    }
    HAL_GPIO_TogglePin(gpio_desc_lut[gpio_pin].port, gpio_desc_lut[gpio_pin].pin);
    return true;
}

bool GPIO_Driver_WritePin (eGpioPin_t gpio_pin, eGpioPinState_t pin_state) {
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

bool GPIO_Driver_ReadPin (eGpioPin_t gpio_pin, eGpioPinState_t *pin_status) {
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

bool GPIO_Driver_InitAll (void) {
    eGpioAllPin_t all_good = eGpioAllPinsOK;
    for (eGpioPin_t pin = eGpioPinFirst; pin < eGpioPinLast; pin++) {
        if (GPIO_Driver_Init(pin) == false) {
            all_good = eGpioAllPinsNotOk;
        }
    }
    if (all_good == eGpioAllPinsNotOk) {
        return false;
    }
    return true;
}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
}
// /**********************************************************************************************************************
//  * Definitions of exported functions
//  *********************************************************************************************************************/
