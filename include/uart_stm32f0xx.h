#ifndef UART_STM32F0XX_H
#define UART_STM32F0XX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "uart_shared.h"
#include "stm32f0xx_hal.h"

#define UART_STM32F0XX_NO_OF_UARTS 2        // Počet dostupných UARTů na STM32F0xx

/* Definice maximálních velikostí bufferů (počet prvků velikosti uart_data_t) */
#define UART_STM32F0XX_BUFFER_MAX_LENGTH_TX 32
#define UART_STM32F0XX_BUFFER_MAX_LENGTH_RX 32

typedef struct s_uart_handle_stm32f0xx {
    UART_HandleTypeDef uart_handle_HAL;     // HAL handle pro konkretni UART
    uart_data_t *p_rx_buffer;               // Ukazatel na buffer pro prijem dat
    uart_data_t *p_tx_buffer;               // Ukazatel na buffer pro vysilani dat
    uint16_t rx_buffer_length;              // Delka prijimaciho bufferu
    uint16_t tx_buffer_length;              // Delka vysilaciho bufferu
    DMA_HandleTypeDef rx_dma_handle_HAL;    // HAL DMA handle pro prijem dat
    DMA_HandleTypeDef tx_dma_handle_HAL;    // HAL DMA handle pro vysilani dat
    GPIO_InitTypeDef rx_gpio_init_struct;   // Struktura pro konfiguraci RX GPIO
    GPIO_TypeDef* p_rx_port;                // Ukazatel na RX GPIO port
    GPIO_InitTypeDef tx_gpio_init_struct;   // Struktura pro konfiguraci TX GPIO
    GPIO_TypeDef* p_tx_port;                // Ukazatel na TX GPIO port
} s_uart_handle_stm32f0xx_t;


typedef struct s_usarts_data_stm32f0xx{
    uint16_t rx_char;
    s_uart_handle_stm32f0xx_t* p_uart_handle_stm32f0xx;
} s_usarts_data_stm32f0xx_t;


void uart_irq_handler_stm32f0xx(uint8_t a_index);
void uart_dma_irq_handler_stm32f0xx(uint8_t a_index);
e_uart_result_t uart_create_handle_stm32f0xx(s_uart_handle_stm32f0xx_t **app_handle);
e_uart_result_t uart_create_stm32f0xx(void* ap_handle);
e_uart_result_t uart_destroy_stm32f0xx(void* ap_handle);
e_uart_result_t uart_open_stm32f0xx(void* ap_handle);
e_uart_result_t uart_close_stm32f0xx(void* ap_handle);
e_uart_result_t uart_send_stm32f0xx(void* ap_handle, const uint16_t* ap_data, size_t a_length);
e_uart_result_t uart_receive_stm32f0xx(void* ap_handle, uint16_t* ap_buffer, size_t a_buffer_size, size_t* ap_received_length);


#ifdef __cplusplus
}
#endif

#endif // UART_STM32F0XX_H
