#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "uart_stm32f0xx.h"

s_usarts_data_stm32f0xx_t usarts_data_stm32f0xx[UART_STM32F0XX_NO_OF_UARTS] = {0};


void uart_irq_handler_stm32f0xx(uint8_t a_index)
{
    // Docasny ukazatel na handle pro konkretni UART
    s_uart_handle_stm32f0xx_t* p_uart_handle = NULL;
    // Index v rozsahu?
    if (UART_STM32F0XX_NO_OF_UARTS > a_index)
    {
        // Získej handle
        p_uart_handle = usarts_data_stm32f0xx[a_index].p_uart_handle_stm32f0xx;
        // Kontrola handle → Instance
        if (NULL != p_uart_handle)
        {
            if (NULL != p_uart_handle->uart_handle_HAL.Instance)
            {
                HAL_UART_IRQHandler(&p_uart_handle->uart_handle_HAL);

                /* volitelně: packet-done přes TC
                 * uart_tx_tc_isr(&g_uartX_tx_ctx);
                 */
            }
        }
    }
}

void uart_dma_irq_handler_stm32f0xx(uint8_t a_index)
{
    // Docasny ukazatel na handle pro konkretni UART
    s_uart_handle_stm32f0xx_t* p_uart_handle = NULL;
    // index v rozsahu?
    if (UART_STM32F0XX_NO_OF_UARTS > a_index)
    {
        // získej handle
        p_uart_handle = usarts_data_stm32f0xx[a_index].p_uart_handle_stm32f0xx;
        // Kontrola  handle → Instance → DMA RX
        if (NULL != p_uart_handle)
        {
            if (NULL != p_uart_handle->uart_handle_HAL.Instance)
            {
                if (NULL != p_uart_handle->uart_handle_HAL.hdmarx)
                {
                    HAL_DMA_IRQHandler(p_uart_handle->uart_handle_HAL.hdmarx);
                }

                /* až přidáš TX DMA:
                if (NULL != p_uart_handle->uart_handle_HAL.hdmatx)
                {
                    HAL_DMA_IRQHandler(p_uart_handle->uart_handle_HAL.hdmatx);
                } */
            }
        }
    }
}

void uart_enable_GPIO_clock_stm32f0xx (GPIO_TypeDef *ap_GPIOx) {
    // kontrola platnosti ukazatele
    if (NULL != ap_GPIOx) 
    {
        switch ((uint32_t)ap_GPIOx) {
            case (uint32_t)GPIOA:
                __HAL_RCC_GPIOA_CLK_ENABLE();
                break;
            case (uint32_t)GPIOB:
                __HAL_RCC_GPIOB_CLK_ENABLE();
                break;
            case (uint32_t)GPIOC:
                __HAL_RCC_GPIOC_CLK_ENABLE();
                break;
            case (uint32_t)GPIOD:
                __HAL_RCC_GPIOD_CLK_ENABLE();
                break;
            case (uint32_t)GPIOF:  // Některé STM32F0 mají GPIOF
                __HAL_RCC_GPIOF_CLK_ENABLE();
                break;
            default:
                // Neznámý GPIO port – buď error handler, nebo prázdná akce
                break;
        }
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
    // Ukazatel na handle pro platformovou vrstvu
    s_uart_handle_stm32f0xx_t* p_handle = NULL;
    // Podle HW instance provede inicializaci
    switch  ((uint32_t)(huart->Instance)) 
    {
        case (uint32_t)USART1:

// 2️⃣ Nakonfigurujeme GPIO pro TX a RX
//GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;  // PA9 = TX, PA10 = RX
     
           // Nastavení ukazatele na handle
            p_handle = usarts_data_stm32f0xx[0].p_uart_handle_stm32f0xx;
            // Kontrola ukazatele
            if (NULL != p_handle) {
                // Povolime hodiny pro USART1
                __HAL_RCC_USART1_CLK_ENABLE();
                // Povolime hodiny pro GPIO porty
                uart_enable_GPIO_clock_stm32f0xx(p_handle->p_rx_port);
                uart_enable_GPIO_clock_stm32f0xx(p_handle->p_tx_port);
                // Provedeme inicializaci GPIO
                HAL_GPIO_Init(p_handle->p_rx_port, &p_handle->rx_gpio_init_struct);
                HAL_GPIO_Init(p_handle->p_tx_port, &p_handle->tx_gpio_init_struct);  
                // Povolime hodiny pro DMA 
                __HAL_RCC_DMA1_CLK_ENABLE();
                // Konfigurace DMA pro prijem dat
                p_handle->rx_dma_handle_HAL.Instance = DMA1_Channel3;
                p_handle->rx_dma_handle_HAL.Init.Direction = DMA_PERIPH_TO_MEMORY;
                p_handle->rx_dma_handle_HAL.Init.PeriphInc = DMA_PINC_DISABLE;
                p_handle->rx_dma_handle_HAL.Init.MemInc = DMA_MINC_ENABLE;
                switch (sizeof(uart_data_t))
                {
                    case 1:
                        p_handle->rx_dma_handle_HAL.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;     // 8-bit => BYTE
                        p_handle->rx_dma_handle_HAL.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;        // 8-bit => BYTE
                        break;
                    case 2:
                        p_handle->rx_dma_handle_HAL.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // 16-bit => HALFWORD
                        p_handle->rx_dma_handle_HAL.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;    // 16-bit => HALFWORD
                        break;
                    case 4:
                        p_handle->rx_dma_handle_HAL.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;     // 32-bit => WORD
                        p_handle->rx_dma_handle_HAL.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;        // 32-bit => WORD
                        break;
                    default:
                        p_handle->rx_dma_handle_HAL.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;     // 8-bit => BYTE
                        p_handle->rx_dma_handle_HAL.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;        // 8-bit => BYTE
                        // Nepodporovana velikost dat
                        LOG(LOG_MODULE_UART, LOG_LEVEL_ERROR, "Nepodporovana velikost dat pro DMA\r\n");
                }
                p_handle->rx_dma_handle_HAL.Init.Mode = DMA_CIRCULAR;                           // kruhový RX
                p_handle->rx_dma_handle_HAL.Init.Priority = DMA_PRIORITY_HIGH;
                
                // Inicializace DMA            
                if (HAL_OK == HAL_DMA_Init(&p_handle->rx_dma_handle_HAL))
                {
                    // Propojení DMA s UART handlem (důležité!)
                    __HAL_LINKDMA(&p_handle->uart_handle_HAL, hdmarx, p_handle->rx_dma_handle_HAL);
                
                    // F0: pro kanály 2 a 3 je sdružený vektor
                    HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
                    // Povolime preruseni
                    HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

                    // Nastavime prioritu preruseni
                    HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
                    // Povolime preruseni
                    HAL_NVIC_EnableIRQ(USART1_IRQn);
                }
                else
                {
                      // Nepovedlo se inicializovat DMA
                      LOG(LOG_MODULE_UART, LOG_LEVEL_ERROR, "Nepovedlo se inicializovat DMA\r\n");
                }
            }
            break;
        case (uint32_t) USART2:
            // Nastavení ukazatele na handle
            p_handle = usarts_data_stm32f0xx[1].p_uart_handle_stm32f0xx;
            // Kontrola ukazatele
            if (NULL != p_handle) {
                // Povolime hodiny pro USART2
                __HAL_RCC_USART2_CLK_ENABLE();
                // Povolime hodiny pro GPIO porty
                uart_enable_GPIO_clock_stm32f0xx(p_handle->p_rx_port);
                uart_enable_GPIO_clock_stm32f0xx(p_handle->p_tx_port);
                // Provedeme inicializaci GPIO
                HAL_GPIO_Init(p_handle->p_rx_port, &p_handle->rx_gpio_init_struct);
                HAL_GPIO_Init(p_handle->p_tx_port, &p_handle->tx_gpio_init_struct);  
                
                // Povolime hodiny pro DMA 
                __HAL_RCC_DMA1_CLK_ENABLE();
                // Konfigurace DMA pro prijem dat
                p_handle->rx_dma_handle_HAL.Instance = DMA1_Channel5; 
                p_handle->rx_dma_handle_HAL.Init.Direction = DMA_PERIPH_TO_MEMORY;
                p_handle->rx_dma_handle_HAL.Init.PeriphInc = DMA_PINC_DISABLE;
                p_handle->rx_dma_handle_HAL.Init.MemInc = DMA_MINC_ENABLE;
                switch (sizeof(uart_data_t))
                {
                    case 1:
                        p_handle->rx_dma_handle_HAL.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;     // 8-bit => BYTE
                        p_handle->rx_dma_handle_HAL.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;        // 8-bit => BYTE
                        break;
                    case 2:
                        p_handle->rx_dma_handle_HAL.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // 16-bit => HALFWORD
                        p_handle->rx_dma_handle_HAL.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;    // 16-bit => HALFWORD
                        break;
                    case 4:
                        p_handle->rx_dma_handle_HAL.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;     // 32-bit => WORD
                        p_handle->rx_dma_handle_HAL.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;        // 32-bit => WORD
                        break;
                    default:
                        p_handle->rx_dma_handle_HAL.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;     // 8-bit => BYTE
                        p_handle->rx_dma_handle_HAL.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;        // 8-bit => BYTE
                        // Nepodporovana velikost dat
                        LOG(LOG_MODULE_UART, LOG_LEVEL_ERROR, "Nepodporovana velikost dat pro DMA\r\n");
                }
                p_handle->rx_dma_handle_HAL.Init.Mode = DMA_CIRCULAR;                           // kruhový RX
                p_handle->rx_dma_handle_HAL.Init.Priority = DMA_PRIORITY_HIGH;  
                
                // Inicializace DMA            
                if (HAL_OK == HAL_DMA_Init(&p_handle->rx_dma_handle_HAL))
                {
                    // Propojení DMA s UART handlem (důležité!)
                    __HAL_LINKDMA(&p_handle->uart_handle_HAL, hdmarx, p_handle->rx_dma_handle_HAL);
                
                    // F0: pro kanály 4 a 5 je sdružený vektor
                    HAL_NVIC_SetPriority(DMA1_Channel4_5_IRQn, 0, 0);
                    // Povolime preruseni
                    HAL_NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);
                    
                    // Nastavime prioritu preruseni
                    HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);
                    // Povolime preruseni
                    HAL_NVIC_EnableIRQ(USART2_IRQn);
                }
                else
                {
                      // Nepovedlo se inicializovat DMA
                      LOG(LOG_MODULE_UART, LOG_LEVEL_ERROR, "Nepovedlo se inicializovat DMA\r\n");
                }
            }
            break;
        default:
            // Nenasel handle
            break;
    }
}

e_uart_result_t uart_create_handle_stm32f0xx(s_uart_handle_stm32f0xx_t **app_uart_handle_stm32f0xx)
{
    // Vysledek operace
    e_uart_result_t result = e_UART_RESULT_OK;
    
    // Kontrola parametru
    if (NULL != app_uart_handle_stm32f0xx) 
    {
        // Alokování paměti pro handle
        *app_uart_handle_stm32f0xx = (s_uart_handle_stm32f0xx_t *) malloc(sizeof(s_uart_handle_stm32f0xx_t));
        // Kontrola alokace paměti
        if (NULL != *app_uart_handle_stm32f0xx)    
        {
            // Nulování struktury
            memset(*app_uart_handle_stm32f0xx, 0, sizeof(s_uart_handle_stm32f0xx_t));
        }
        else        
        {
            // Chyba alokace paměti
            result = e_UART_RESULT_MEMORY_ERROR;
        }
    }
    else
    {
        // Neplatné parametry
        result = e_UART_RESULT_INVALID_PARAM;
    }
    // Vrácení výsledku
    return result;
}

e_uart_result_t uart_create_stm32f0xx(void* ap_uart_handle_stm32f0xx)
{
    // Vysledek operace
    e_uart_result_t result = e_UART_RESULT_OK;
    s_uart_handle_stm32f0xx_t* p_uart_handle_stm32f0xx = NULL;

    // Kontrola parametru
    if (NULL == ap_uart_handle_stm32f0xx)
    {
        // Neplatné parametry
        result = e_UART_RESULT_INVALID_PARAM;
    }

    if (e_UART_RESULT_OK == result)
    {
        // Pretypovani handle na ukazatel na strukturu platformy
        p_uart_handle_stm32f0xx = (s_uart_handle_stm32f0xx_t *) ap_uart_handle_stm32f0xx;
        // Ověření parametru konfigurace UARTu
        if ((UART_STM32F0XX_BUFFER_MAX_LENGTH_RX < p_uart_handle_stm32f0xx->rx_buffer_length) ||
            (UART_STM32F0XX_BUFFER_MAX_LENGTH_TX < p_uart_handle_stm32f0xx->tx_buffer_length) ||
            (0 == p_uart_handle_stm32f0xx->rx_buffer_length) ||
            (0 == p_uart_handle_stm32f0xx->tx_buffer_length))
        {
            // Neplatná velikost bufferu
            result = e_UART_RESULT_INVALID_PARAM;
        }
    }

    if (e_UART_RESULT_OK == result)
    {
        // Kontrola platnosti ukazatelů na GPIO porty a piny
        if ((NULL == p_uart_handle_stm32f0xx->p_rx_port) ||
            (NULL == p_uart_handle_stm32f0xx->p_tx_port) ||
            (0u   == p_uart_handle_stm32f0xx->rx_gpio_init_struct.Pin) ||
            (0u   == p_uart_handle_stm32f0xx->tx_gpio_init_struct.Pin))
        {
            // Neplatné parametry
            result = e_UART_RESULT_INVALID_PARAM;
        }
    }

    if (e_UART_RESULT_OK == result)
    {
        // Nastavení handle pro platformovou vrstvu
        switch ((uint32_t)(p_uart_handle_stm32f0xx->uart_handle_HAL.Instance)) {
            case (uint32_t) USART1:
                usarts_data_stm32f0xx[0].p_uart_handle_stm32f0xx = p_uart_handle_stm32f0xx;
                break;
            case (uint32_t) USART2:
                usarts_data_stm32f0xx[1].p_uart_handle_stm32f0xx = p_uart_handle_stm32f0xx;
                break;
            default:
                // Inicializace se nezdarila
                result = e_UART_RESULT_ERROR;
                break;
        }
    }

    if (e_UART_RESULT_OK == result)
    {
        // Alokace paměti pro RX buffer
        p_uart_handle_stm32f0xx->p_rx_buffer = (uart_data_t *) malloc(p_uart_handle_stm32f0xx->rx_buffer_length * sizeof(uart_data_t));
        // Kontrola alokace paměti
        if (NULL == p_uart_handle_stm32f0xx->p_rx_buffer)
        {
            result = e_UART_RESULT_MEMORY_ERROR;
        }
    }

    if (e_UART_RESULT_OK == result)
    {
        // Alokace paměti pro TX buffer
        p_uart_handle_stm32f0xx->p_tx_buffer = (uart_data_t *) malloc(p_uart_handle_stm32f0xx->tx_buffer_length * sizeof(uart_data_t));
        // Kontrola alokace paměti
        if (NULL == p_uart_handle_stm32f0xx->p_tx_buffer)
        {
            result = e_UART_RESULT_MEMORY_ERROR;
        }
    }

    if (e_UART_RESULT_OK == result)
    {
        // Zavola inicializaci HW instance
        if (HAL_OK != HAL_UART_Init(&p_uart_handle_stm32f0xx->uart_handle_HAL)) 
        {
            // Inicializace se nezdarila
            result = e_UART_RESULT_ERROR;
        }
    }

    if (e_UART_RESULT_OK != result)
    {
        // Chyba - uklid
        // Uvolnění alokované paměti pokudje platný handle
        if (NULL != p_uart_handle_stm32f0xx)
        {
            // Uvolnění rx fifo
            if (NULL != p_uart_handle_stm32f0xx->p_rx_buffer)
            {
                // Uvolnění rx fifo
                free(p_uart_handle_stm32f0xx->p_rx_buffer);
            }
            
            // Uvolnění tx fifo
            if (NULL != p_uart_handle_stm32f0xx->p_tx_buffer)
            {
                // Uvolnění tx fifo
                free(p_uart_handle_stm32f0xx->p_tx_buffer);
            }
        }
    }
    //  Vraceni vysledku
    return result;
}

e_uart_result_t uart_destroy_stm32f0xx(void* ap_handle)
{
    e_uart_result_t result = e_UART_RESULT_OK;
    //  Vraceni vysledku
    return result;
}

e_uart_result_t uart_open_stm32f0xx(void* ap_handle)
{
    e_uart_result_t result = e_UART_RESULT_OK;
    //  Vraceni vysledku
    return result;
}

e_uart_result_t uart_close_stm32f0xx(void* ap_handle)
{
    e_uart_result_t result = e_UART_RESULT_OK;
    //  Vraceni vysledku
    return result;
}

e_uart_result_t uart_send_stm32f0xx(void* ap_handle, const uint16_t* ap_data, size_t a_length)
{
    e_uart_result_t result = e_UART_RESULT_OK;
    //  Vraceni vysledku
    return result;
}

e_uart_result_t uart_receive_stm32f0xx(void* ap_handle, uint16_t* ap_buffer, size_t a_buffer_size, size_t* ap_received_length)
{
    e_uart_result_t result = e_UART_RESULT_OK;
    //  Vraceni vysledku
    return result;
}
