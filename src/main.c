#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"

#include <string.h>  // Pro memcpy
#include <stdlib.h>  // Pro malloc a free


#include "main.h"
#include "uart_stm32f0xx.h"
#include "uart.h"
//#include "uartx.h"
#include "log.h"

void disable_interrupts(void) {
    __disable_irq();  // Zakáže všechna přerušení
}

void enable_interrupts(void) {
    __enable_irq();   // Obnoví přerušení
}

// 🕒 SysTick Interrupt Handler (nutné pro HAL_Delay)
void SysTick_Handler(void) {
    HAL_IncTick();  // ✅ Inkrementace časovače HAL
}

void Error_Handler(void) {
    while (1) {
        // Nekonečná smyčka pro zastavení programu
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);    
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /*** 1️⃣ Nakonfigurujeme oscilátor ***/
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI; // Použijeme interní HSI
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;                  // Zapneme HSI (8 MHz)
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT; // Výchozí kalibrace
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;              // Zapneme PLL
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;      // Zdroj PLL = HSI (8 MHz)
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;              // Násobíme 6x → 8 MHz × 6 = 48 MHz

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler(); // Pokud se konfigurace nepodaří, vyvoláme chybu
    }

    /*** 2️⃣ Nakonfigurujeme hodiny ***/
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | 
                                   RCC_CLOCKTYPE_PCLK1);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // SYSCLK = PLL (48 MHz)
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;        // HCLK = SYSCLK / 1 = 48 MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;         // PCLK1 = HCLK / 1 = 48 MHz

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler();
    }
}

void GPIO_Init(void) {
    __HAL_RCC_GPIOC_CLK_ENABLE();  // 🟢 Povolit hodiny pro GPIOC

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

// Funkce pro přesměrování logu na UART
void log_output_uart(const char *ap_message) {
/*    if (uratx_SendBuffer((uint8_t *) ap_message, strlen(ap_message)) != 0) {
        Error_Handler();
    }*/
}

int main(void) {
    // 🟢 Inicializace HAL knihovny
    HAL_Init();

    // 🕒 Konfigurace systémových hodin
    SystemClock_Config();

    // 🟢 Inicializace GPIO
    GPIO_Init();


    //uratx_Init();

    //uint8_t length;

//    char messageTx[] = "Uart start:\r\n";
//    char messageRx[256];

    uint32_t oldTime;
    uint32_t newTime;

    // Nastavení logovací úrovně a výstupu na UART
    log_set_global_level(LOG_LEVEL_DEBUG);
    log_set_module_level(LOG_MODULE_MAIN, LOG_LEVEL_DEBUG);
//    log_set_output_function(log_output_uart);

    // Testovací logování
    LOG(LOG_MODULE_MAIN, LOG_LEVEL_ERROR, "Toto je test");
    //LOG(LOG_MODULE_UART, LOG_LEVEL_INFO, "UART inicializován.");
    //LOG(LOG_MODULE_SPI, LOG_LEVEL_DEBUG, "SPI přenos zahájen."); // Nebude zobrazen (DEBUG je nad INFO)

/*    if (uratx_SendBuffer((uint8_t *) messageTx, strlen(messageTx)) != 0) 
    {
        Error_Handler();
    }
*/

    // --------------------------------------------------------------
    // Inicializace platformově závislé části UARTu
    // --------------------------------------------------------------
    
    // Ukazatel na strukturu pro platformově závislou část UARTu - handler
    s_uart_handle_stm32f0xx_t *p_uart_handle_stm32f0xx = NULL;

    // Vytvoření handleru pro platformově závislou část UARTu
    if (e_UART_RESULT_OK == uart_create_handle_stm32f0xx(&p_uart_handle_stm32f0xx)) 
    {
        // Nastaveni parametru UARTu platformove zavisle casti
    
        // Vybereme si konkretni HW instanci UARTu
        p_uart_handle_stm32f0xx->uart_handle_HAL.Instance = USART2;
        // Nastaveni parametru platformove zavisle casti UARTu 
        p_uart_handle_stm32f0xx->uart_handle_HAL.Init.BaudRate = 115200;
        p_uart_handle_stm32f0xx->uart_handle_HAL.Init.WordLength = UART_WORDLENGTH_8B;
        p_uart_handle_stm32f0xx->uart_handle_HAL.Init.StopBits = UART_STOPBITS_1;
        p_uart_handle_stm32f0xx->uart_handle_HAL.Init.Parity = UART_PARITY_NONE;
        p_uart_handle_stm32f0xx->uart_handle_HAL.Init.Mode = UART_MODE_TX_RX;
        p_uart_handle_stm32f0xx->uart_handle_HAL.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        p_uart_handle_stm32f0xx->uart_handle_HAL.Init.OverSampling = UART_OVERSAMPLING_16;
        // Nastaveni GPIO pinu pro TX
        p_uart_handle_stm32f0xx->tx_gpio_init_struct.Pin = GPIO_PIN_2;
        p_uart_handle_stm32f0xx->tx_gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        p_uart_handle_stm32f0xx->tx_gpio_init_struct.Pull = GPIO_PULLUP;
        p_uart_handle_stm32f0xx->tx_gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        p_uart_handle_stm32f0xx->tx_gpio_init_struct.Alternate = GPIO_AF1_USART2;
        p_uart_handle_stm32f0xx->p_tx_port = GPIOA;
        // Nastaveni GPIO pinu pro RX
        p_uart_handle_stm32f0xx->rx_gpio_init_struct.Pin = GPIO_PIN_3;
        p_uart_handle_stm32f0xx->rx_gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        p_uart_handle_stm32f0xx->rx_gpio_init_struct.Pull = GPIO_PULLUP;
        p_uart_handle_stm32f0xx->rx_gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        p_uart_handle_stm32f0xx->rx_gpio_init_struct.Alternate = GPIO_AF1_USART2;
        p_uart_handle_stm32f0xx->p_rx_port = GPIOA;
        // Nastaveni delky bufferu
        p_uart_handle_stm32f0xx->tx_buffer_length = UART_STM32F0XX_BUFFER_MAX_LENGTH_TX;
        p_uart_handle_stm32f0xx->rx_buffer_length = UART_STM32F0XX_BUFFER_MAX_LENGTH_RX;
    }
    else
    {
        Error_Handler();
    }
    
    // --------------------------------------------------------------
    // Inicializace platformově nezávislé části UARTu
    // --------------------------------------------------------------
    
    // Ukazatel na strukturu pro UART - handler
    s_uart_handle_t *p_uart_handle = NULL;
    
    // Struktura pro konfigurace UARTu
    s_uart_config_t uart_config;
    
    // Nastaveni parametru UARTu
    uart_config.rx_buffer_length = 128;
    uart_config.tx_buffer_length = 128;
    uart_config.fp_platform_create = uart_create_stm32f0xx;
    uart_config.fp_platform_destroy = uart_destroy_stm32f0xx;
    uart_config.fp_platform_open = uart_open_stm32f0xx;
    uart_config.fp_platform_close = uart_close_stm32f0xx;
    uart_config.fp_platform_send = uart_send_stm32f0xx;
    uart_config.fp_platform_receive = uart_receive_stm32f0xx;
    uart_config.p_platform_handle = (void *) p_uart_handle_stm32f0xx;

    // Vytvoření UARTu
    if (e_UART_RESULT_OK != uart_create(&p_uart_handle, &uart_config))
    {
        Error_Handler();
    }



    // --------------------------------------------------------------
    oldTime = HAL_GetTick();
    // 🌟 Hlavní smyčka
    while (1) {
        
        newTime = HAL_GetTick();
        if ((newTime - oldTime) >= 500){
            oldTime = newTime;
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
        }

/*        if (uratx_GetBuffer((uint8_t *) messageRx, &length) != 0) {
            Error_Handler();
        }    

        if (length != 0) {   
            if (uratx_SendBuffer((uint8_t *) messageRx, length) != 0) {
                Error_Handler();
            }
        }*/
    }
}
