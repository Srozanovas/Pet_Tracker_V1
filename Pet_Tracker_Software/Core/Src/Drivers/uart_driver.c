
/*_______________________________________________INCLUDES____________________________________________________*/
#include "uart_driver.h"
#include "../utility/ring_bufer.h"
#include "main.h"
/*_______________________________________________INCLUDES____________________________________________________*/




/*____________________________________________DEFINES, CONSTANTS, LUT's, VARIABLES, etc______________________________________________*/
#define RING_BUFFER_SIZE 150
#define UART_TX_MAX 300


typedef struct sUartDesc_t {
    USART_TypeDef *instance;
    uint32_t word_length;
    uint32_t stop_bits;
    uint32_t parity;
    uint32_t uart_mode;
    uint32_t hardware_flow_control;
    uint32_t over_sampling;
    uint32_t one_bit_sampling;
    uint32_t advanced_feature;
    eGpioPin_t tx_pin;
    eGpioPin_t rx_pin;
    IRQn_Type irqn;
    uint8_t priority;
    uint8_t priority_group;
    uint32_t uart_flag;
} sUartDesc_t;

typedef struct sUartDescDynamic_t {
    UART_HandleTypeDef uart_handle;   
    sRingBuffer_t *rb_handle;
    uint32_t baudRate;
    uint8_t rx_data[1]; 
    bool (*uart_set_flag)(eUart_t);
} sUartDescDynamic_t;

const static sUartDesc_t uart_desc_lut[] = {
    [eUartModem] = {.instance       = USART1, 
                    .word_length    = UART_WORDLENGTH_8B, 
                    .stop_bits      = UART_STOPBITS_1, 
                    .parity         = UART_PARITY_NONE, 
                    .uart_mode      = UART_MODE_TX_RX, 
                    .hardware_flow_control  = UART_HWCONTROL_NONE, 
                    .over_sampling          = UART_OVERSAMPLING_16, 
                    .tx_pin         = eGpioPinB6UART1TX, 
                    .rx_pin         = eGpioPinB7UART1RX, 
                    .irqn           = USART1_IRQn, 
                    .priority       = 30, 
                    .priority_group = 1, 
                },
    
    [eUartDebug] = {.instance       = USART2, 
                    .word_length    = UART_WORDLENGTH_8B,
                    .stop_bits      = UART_STOPBITS_1, 
                    .parity         = UART_PARITY_NONE, 
                    .uart_mode      = UART_MODE_TX_RX, 
                    .hardware_flow_control  = UART_HWCONTROL_NONE, 
                    .over_sampling          = UART_OVERSAMPLING_16, 
                    .tx_pin         = eGpioPinA2UART2TX, 
                    .rx_pin         = eGpioPinA3UART2RX, 
                    .irqn           = USART2_IRQn, 
                    .priority       = 30, 
                    .priority_group = 1, 
                },
};

static sUartDescDynamic_t uart_desc_lut_dynamic[] = {
     [eUartDebug] = {.rb_handle = NULL,  .uart_handle = {0},   .baudRate = eBaudRateLast, .uart_set_flag = NULL,     .rx_data = 0},
     [eUartModem] = {.rb_handle = NULL,  .uart_handle = {0},   .baudRate = eBaudRateLast, .uart_set_flag = NULL,     .rx_data = 0},
};

const static uint32_t uart_baudrate_lut[] = {
    [eBaudRate1200] = 1200,
    [eBaudRate2400] = 2400,
    [eBaudRate4800] = 4800,
	[eBaudRate9600] = 9600,
    [eBaudRate19200] = 19200,
    [eBaudRate38400] = 38400,
    [eBaudRate57600] = 57600,
    [eBaudRate115200] = 115200,
};


#ifdef DEBUGO_KODAS
    uint8_t tx_pc[50]; 
    uint8_t tx_pc_index = 0; 
    uint8_t tx_modem[50]; 
    uint8_t tx_modem_index = 0; 
#endif



/*____________________________________________CONSTANTS, LUT's, VARIABLES, etc______________________________________________*/




bool UART_Driver_Init (eUart_t uart, eBaudRate_t baudrate, bool(*function_pointer)(eUart_t)) { //INITIALIZE UART 
    if (uart >= eUartLast) {
        return false;
    }
    if (baudrate >= eBaudRateLast) {
        return false;
    }
    if (UART_Driver_Low_Level_Init(uart) == false){ 
        return false; 
    }
    uart_desc_lut_dynamic[uart].uart_handle.Instance = uart_desc_lut[uart].instance;
    uart_desc_lut_dynamic[uart].uart_handle.Init.BaudRate = uart_baudrate_lut[baudrate]; 
    uart_desc_lut_dynamic[uart].uart_handle.Init.WordLength = uart_desc_lut[uart].word_length; 
    uart_desc_lut_dynamic[uart].uart_handle.Init.StopBits = uart_desc_lut[uart].stop_bits;
    uart_desc_lut_dynamic[uart].uart_handle.Init.Parity = uart_desc_lut[uart].parity; 
    uart_desc_lut_dynamic[uart].uart_handle.Init.Mode =  uart_desc_lut[uart].uart_mode; 
    uart_desc_lut_dynamic[uart].uart_handle.Init.HwFlowCtl = uart_desc_lut[uart].hardware_flow_control; 
    uart_desc_lut_dynamic[uart].uart_handle.Init.OverSampling = uart_desc_lut[uart].over_sampling; 
    uart_desc_lut_dynamic[uart].uart_handle.Init.OneBitSampling = uart_desc_lut[uart].one_bit_sampling; 
    uart_desc_lut_dynamic[uart].uart_handle.AdvancedInit.AdvFeatureInit = uart_desc_lut[uart].advanced_feature; 

    if (HAL_UART_Init(&uart_desc_lut_dynamic[uart].uart_handle) != HAL_OK)
    {
        Error_Handler();
    }
    uart_desc_lut_dynamic[uart].uart_set_flag = function_pointer;

    HAL_UART_Receive_IT(&uart_desc_lut_dynamic[uart].uart_handle, uart_desc_lut_dynamic[uart].rx_data, 1); 
    return true; 

}

bool UART_Driver_Low_Level_Init(eUart_t uart)
{
    //INIT CLOCKS AND RING BUFFER
    if (GPIO_Driver_Init(uart_desc_lut[uart].tx_pin, ePinLow) == false) {
        return false;
    }
    if (GPIO_Driver_Init(uart_desc_lut[uart].rx_pin, ePinLow) == false) {
        return false;
    }
    if (RingBuffer_Init(&uart_desc_lut_dynamic[uart].rb_handle, RING_BUFFER_SIZE) == false) {
        return false;
    }

    //ENABLE CLOCKS AND INTERUPTS
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    if(uart==eUartModem) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
        PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK){
            Error_Handler();
        }
        __HAL_RCC_USART1_CLK_ENABLE();
        HAL_NVIC_SetPriority(USART1_IRQn, 30, 1);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
        return true; 
    }

    if(uart==eUartDebug){
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
        PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
            Error_Handler();
        }
        __HAL_RCC_USART2_CLK_ENABLE();
        HAL_NVIC_SetPriority(USART2_IRQn, 30, 1);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
        return true; 
    }
    return false; 
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){ //SAVE 1 BYTE OF DATA TO RING BUFFER
    if((*huart).Instance==uart_desc_lut_dynamic[eUartDebug].uart_handle.Instance){
    	#ifdef DEBUGO_KODAS
        tx_modem[tx_modem_index] = uart_desc_lut_dynamic[eUartDebug].rx_data[0]; 
        if (tx_modem[tx_modem_index]=='\n') {
            UART_Driver_Send_String(eUartModem, tx_modem, tx_modem_index); 
            tx_modem_index = 0 ;
        } else { 
            tx_modem_index++; 
        }
        HAL_UART_Receive_IT(&uart_desc_lut_dynamic[eUartDebug].uart_handle, uart_desc_lut_dynamic[eUartDebug].rx_data, 1);

        #else 
        if (uart_desc_lut_dynamic[eUartDebug].rx_data[0]!=0)
    		RingBuffer_Put(uart_desc_lut_dynamic[eUartDebug].rb_handle, uart_desc_lut_dynamic[eUartDebug].rx_data[0]);
        if (uart_desc_lut_dynamic[eUartDebug].rx_data[0] == (uint8_t)('\n')) {
            uart_desc_lut_dynamic[eUartDebug].uart_set_flag(eUartDebug);
        }
        HAL_UART_Receive_IT(&uart_desc_lut_dynamic[eUartDebug].uart_handle, uart_desc_lut_dynamic[eUartDebug].rx_data, 1);
        #endif
    } 
    if((*huart).Instance==uart_desc_lut_dynamic[eUartModem].uart_handle.Instance){
    	#ifdef DEBUGO_KODAS
        tx_pc[tx_pc_index] = uart_desc_lut_dynamic[eUartDebug].rx_data[0]; 
        if (tx_pc[tx_pc_index]=='\n') {
            UART_Driver_Send_String(eUartDebug, tx_pc, tx_pc_index); 
            tx_pc_index = 0 ;
        } else { 
            tx_pc_index++; 
        }
        HAL_UART_Receive_IT(&uart_desc_lut_dynamic[eUartDebug].uart_handle, uart_desc_lut_dynamic[eUartDebug].rx_data, 1);

        #else 
        if (uart_desc_lut_dynamic[eUartModem].rx_data[0]!=0)
    		RingBuffer_Put(uart_desc_lut_dynamic[eUartModem].rb_handle, uart_desc_lut_dynamic[eUartModem].rx_data[0]);
        if ((uart_desc_lut_dynamic[eUartModem].rx_data[0] == (uint8_t)('\n'))
            ||uart_desc_lut_dynamic[eUartModem].rx_data[0] == (uint8_t)('>')) {
            uart_desc_lut_dynamic[eUartModem].uart_set_flag(eUartModem);
        }
        HAL_UART_Receive_IT(&uart_desc_lut_dynamic[eUartModem].uart_handle, uart_desc_lut_dynamic[eUartModem].rx_data, 1);
        #endif
    } 
 }


bool UART_Driver_GetByte (eUart_t uart, uint8_t *byte) {    //GET 1 BYTE OF DATA FROM RING BUFFER
    if (RingBuffer_Get(uart_desc_lut_dynamic[uart].rb_handle, byte) == false) {
        return false;
    }
    return true;
}





bool UART_Driver_Send_String (eUart_t uart, uint8_t *uart_tx, uint16_t size) {   //SEND MESSAGE THROUGH UART
    uint8_t tx_size;
    for (tx_size=0; (tx_size<UART_TX_MAX) && (tx_size < size); tx_size ++){
    	if (*(uart_tx+tx_size) == 0) break;
    }
	HAL_UART_Transmit(&uart_desc_lut_dynamic[uart].uart_handle, uart_tx, tx_size, 50);
    return true;
}


/*_________________________INTERUPT HANDLERS______________________________________*/

void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&uart_desc_lut_dynamic[eUartDebug].uart_handle);
}
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&uart_desc_lut_dynamic[eUartModem].uart_handle);
}

