#include <stdlib.h>
#include <string.h>

#include "uart.h"

e_uart_result_t uart_create(s_uart_handle_t **app_uart_handle, const s_uart_config_t *ap_uart_config)
{
    // Vysledek operace
    e_uart_result_t result = e_UART_RESULT_OK;
    s_uart_handle_t *p_uart_handle = NULL;
    s_fifo_config_t fifo_config;

    // Kontrola parametru
    if ((NULL == ap_uart_config) ||
        (NULL == app_uart_handle)) 
    {
        // Neplatné parametry
        result = e_UART_RESULT_INVALID_PARAM;
    }

    if (e_UART_RESULT_OK == result) 
    {
        // Ověření parametru konfigurace UARTu
        if ((UART_BUFFER_LENGTH_RX < ap_uart_config->rx_buffer_length) ||
            (UART_BUFFER_LENGTH_TX < ap_uart_config->tx_buffer_length) ||
            (0 == ap_uart_config->rx_buffer_length) ||
            (0 == ap_uart_config->tx_buffer_length) ||
            (NULL == ap_uart_config->fp_platform_create) ||
            (NULL == ap_uart_config->fp_platform_destroy) ||
            (NULL == ap_uart_config->fp_platform_open) ||
            (NULL == ap_uart_config->fp_platform_close) ||
            (NULL == ap_uart_config->fp_platform_send) ||
            (NULL == ap_uart_config->fp_platform_receive))
        {
            // Neplatná velikost bufferu nebo ukazatele na funkce
            result = e_UART_RESULT_INVALID_PARAM;
        }
    }

    if (e_UART_RESULT_OK == result) 
    {
        // Alokování paměti pro handle
        p_uart_handle = (s_uart_handle_t *) malloc(sizeof(s_uart_handle_t));
    
        // Kontrola alokace paměti
        if (NULL != p_uart_handle) 
        {
            // Nulování struktury
            memset(p_uart_handle, 0, sizeof(s_uart_handle_t));
        }
        else
        {
            // Chyba alokace paměti
            result = e_UART_RESULT_MEMORY_ERROR;
        }
    }

    if (e_UART_RESULT_OK == result) 
    {
        // konfigurace rx fifo
        fifo_config.length = ap_uart_config->rx_buffer_length;
        fifo_config.item_size = sizeof(uart_data_t);
        fifo_config.is_irq_safe = true;

        // Vytvoření rx fifo bufferu
        if (0 != fifo_create(&p_uart_handle->p_rx_fifo, &fifo_config))
        {
            // Chyba vytváření rx fifo bufferu
            result = e_UART_RESULT_ERROR;          
        }
    }

    if (e_UART_RESULT_OK == result) 
    {
        // konfigurace tx fifo
        fifo_config.length = ap_uart_config->tx_buffer_length;
        fifo_config.item_size = sizeof(uart_data_t);
        fifo_config.is_irq_safe = true;

        // Vytvoření tx fifo bufferu
        if (0 != fifo_create(&p_uart_handle->p_tx_fifo, &fifo_config))
        {
            // Chyba vytváření tx fifo bufferu
            result = e_UART_RESULT_ERROR;          
        }
    }

    if (e_UART_RESULT_OK == result) 
    {
        // Default stav
        p_uart_handle->status = e_UART_STATUS_NOT_INITIALIZED;

        // Kopírování konfigurace
        memcpy(&p_uart_handle->config, ap_uart_config, sizeof(s_uart_config_t));
    
        // Kontrola ukazatele na funkci inicializace platformové vrstvy
        if (NULL != p_uart_handle->config.fp_platform_create)
        {
            // Inicializace platformové vrstvy
            if (e_UART_RESULT_OK == (p_uart_handle->config.fp_platform_create(p_uart_handle->config.p_platform_handle)))
            {
                // Inicializace proběhla v pořádku    
                p_uart_handle->status = e_UART_STATUS_INITIALIZED;
            }
            else
            {
                // Chyba inicializace platformové vrstvy
                result = e_UART_RESULT_ERROR;
            }
        }
        else
        {
            // null ukazatel na funkci inicializace platformové vrstvy
            result = e_UART_RESULT_ERROR;
        }
    }
   
    if (e_UART_RESULT_OK == result)
    {
        // Vse ok - nastavime handle
     
        // Nastavení handle po uspesne inicializaci
        *app_uart_handle = p_uart_handle;
    }
    else
    {
        // Chyba - uklid
    
        // Uvolnění alokované paměti
        if (NULL != p_uart_handle)
        {
            // Uvolnění rx fifo
            if (NULL != p_uart_handle->p_rx_fifo)
            {
                // Uvolnění rx fifo
                if (0 != fifo_destroy(&p_uart_handle->p_rx_fifo))
                {
                    // Nemělo by nastat
                    result = e_UART_RESULT_FATAL_ERROR;
                }
            }

            // Uvolnění tx fifo
            if (NULL != p_uart_handle->p_tx_fifo)
            {
                // Uvolnění tx fifo
                if (0 != fifo_destroy(&p_uart_handle->p_tx_fifo))
                {
                    // Nemělo by nastat
                    result = e_UART_RESULT_FATAL_ERROR;
                }
            }

            // Uvolnění handle
            free(p_uart_handle);
        }
        // Test jestli ukazatel na handle není NULL 
        if (NULL != app_uart_handle) 
        {
            // Nastavi handle na NULL
            *app_uart_handle = NULL;
        }
    }
    
    // Vrácení výsledku
    return result;
}

e_uart_result_t uart_destroy(s_uart_handle_t **app_uart_handle)
{
    e_uart_result_t result = e_UART_RESULT_OK;
    s_uart_handle_t *p_uart_handle = NULL;

    // Kontrola parametru
    if ((NULL == app_uart_handle) || 
        (NULL == *app_uart_handle))
    {
        result = e_UART_RESULT_INVALID_PARAM;
    }

    if (e_UART_RESULT_OK == result) 
    {
        // Ulozeni si ukazatele pro snadnejsi praci
        p_uart_handle = *app_uart_handle;

        // Kontrola stavu
        if (e_UART_STATUS_INITIALIZED != p_uart_handle->status)
        {
            // Chyba - spatny stav
            result = e_UART_RESULT_WRONG_STATE;
        }
    }

    if (e_UART_RESULT_OK == result) 
    {
        // Kontrola ukazatele na destruktor platformove vrstvy
        if (NULL != p_uart_handle->config.fp_platform_destroy)
        {
            // Zavolani destruktoru platformove vrstvy
            if (e_UART_RESULT_OK == (p_uart_handle->config.fp_platform_destroy(p_uart_handle->config.p_platform_handle)))
            {
                // Uvolnění rx fifo
                if (0 != fifo_destroy(&p_uart_handle->p_rx_fifo))
                {
                    // Nemělo by nastat
                    result = e_UART_RESULT_FATAL_ERROR;
                }
                
                // Uvolnění tx fifo
                if (0 != fifo_destroy(&p_uart_handle->p_tx_fifo))
                {
                    // Nemělo by nastat
                    result = e_UART_RESULT_FATAL_ERROR;
                }

                // Uvolnění handle                    
                p_uart_handle->status = e_UART_STATUS_NOT_INITIALIZED;
                free(p_uart_handle);

                // Nastaveni handle na NULL
                *app_uart_handle = NULL;
            }
            else
            {
                // Chyba destrukce platformové vrstvy
                result = e_UART_RESULT_ERROR;
            }
        }
        else
        {
            // Null ukazatel na funkci destruktoru platformové vrstvy
            result = e_UART_RESULT_ERROR;
        }
    }
    
    // Vraceni vysledku
    return result;
}

e_uart_result_t uart_open(s_uart_handle_t *ap_uart_handle)
{
    e_uart_result_t result = e_UART_RESULT_OK;
    
    // Kontrola parametru
    if (NULL == ap_uart_handle)
    {
        result = e_UART_RESULT_INVALID_PARAM;
    }
    
    if (e_UART_RESULT_OK == result) 
    {
        // Kontrola stavu
        if (e_UART_STATUS_INITIALIZED != ap_uart_handle->status)
        {
            result = e_UART_RESULT_WRONG_STATE;
        }
    }
    
    if (e_UART_RESULT_OK == result) 
    {
        // Kontrola ukazatele na funkci otevření platformové vrstvy
        if (NULL != ap_uart_handle->config.fp_platform_open)
        {
            // Zavolani otevření platformove vrstvy
            result = (ap_uart_handle->config.fp_platform_open(ap_uart_handle->config.p_platform_handle));

            // Kontrola výsledku otevření platformové vrstvy
            if (e_UART_RESULT_OK == result)
            {
                // Otevření proběhlo v pořádku
                ap_uart_handle->status = e_UART_STATUS_OPEN;
            }
            else
            {
                // Chyba otevření platformové vrstvy
                result = e_UART_RESULT_ERROR;
            }
        }
        else
        {
            // Null ukazatel na funkci otevření platformové vrstvy
            result = e_UART_RESULT_ERROR;
        }
    }
    
    return result;
}

e_uart_result_t uart_close(s_uart_handle_t *ap_uart_handle)
{
    e_uart_result_t result = e_UART_RESULT_OK;
    
    if (NULL == ap_uart_handle)
    {
        result = e_UART_RESULT_INVALID_PARAM;
    }
    else if (e_UART_STATUS_OPEN != ap_uart_handle->status)
    {
        result = e_UART_RESULT_WRONG_STATE;
    }
    else if (e_UART_RESULT_OK != (ap_uart_handle->config.fp_platform_close(ap_uart_handle->config.p_platform_handle)))
    {
        result = e_UART_RESULT_ERROR;
    }
    else
    {
        ap_uart_handle->status = e_UART_STATUS_INITIALIZED;
    }
    
    return result;
}

e_uart_result_t uart_send(s_uart_handle_t *ap_uart_handle, const uint16_t *ap_data, size_t a_length)
{
    e_uart_result_t result = e_UART_RESULT_OK;
    
    if (NULL == ap_uart_handle)
    {
        result = e_UART_RESULT_INVALID_PARAM;
    }
    else if ((NULL == ap_data) || (0 == a_length))
    {
        result = e_UART_RESULT_INVALID_PARAM;
    }
    else if (e_UART_STATUS_OPEN != ap_uart_handle->status)
    {
        result = e_UART_RESULT_WRONG_STATE;
    }
    else
    {
        result = ap_uart_handle->config.fp_platform_send(ap_uart_handle->config.p_platform_handle, ap_data, a_length);
    }
    
    return result;
}

e_uart_result_t uart_receive(s_uart_handle_t *ap_uart_handle, uint16_t *ap_buffer, size_t a_buffer_size, size_t *ap_received_length)
{
    e_uart_result_t result = e_UART_RESULT_OK;
    
    if (NULL == ap_uart_handle)
    {
        result = e_UART_RESULT_INVALID_PARAM;
    }
    else if ((NULL == ap_buffer) || (0 == a_buffer_size) || (NULL == ap_received_length))
    {
        result = e_UART_RESULT_INVALID_PARAM;
    }
    else if (e_UART_STATUS_OPEN != ap_uart_handle->status)
    {
        result = e_UART_RESULT_WRONG_STATE;
    }
    else
    {
        result = ap_uart_handle->config.fp_platform_receive(ap_uart_handle->config.p_platform_handle, ap_buffer, a_buffer_size, ap_received_length);
    }
    
    return result;
}




