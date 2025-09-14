#include "fifo.h"
#include "main.h"

#include <string.h>  // Pro memcpy
#include <stdlib.h>  // Pro malloc a free


// Funkce pro vytvoření FIFO
int fifo_create(s_fifo_handle_t** app_fifo_handle, s_fifo_config_t* ap_fifo_config)
{
    
    int result = 0;
    s_fifo_handle_t *p_fifo_handle = NULL;
    
    // Kontrola parametru
    if ((NULL == ap_fifo_config) ||
        (NULL == app_fifo_handle)) 
    {
        // Neplatné parametry
        result = -1;  
    }

    if (0 == result) 
    {
        // Ověření parametru konfigurace FIFO
        if ((0 == ap_fifo_config->item_size) || 
            (FIFO_MAX_ITEM_SIZE < ap_fifo_config->item_size) ||
            (0 == ap_fifo_config->length) || 
            (FIFO_MAX_LENGTH < ap_fifo_config->length) 
            )
        {
            // Neplatna konfigurace FIFO
            return -2;  
        }
    }

    if (0 == result) 
    {
        // Alokování paměti pro FIFO handle
        p_fifo_handle = (s_fifo_handle_t *) malloc(sizeof(s_fifo_handle_t));
        
        if (NULL != p_fifo_handle) 
        {
            // Nastavení struktury FIFO
            p_fifo_handle->front = 0;
            p_fifo_handle->rear = 0;
            p_fifo_handle->count = 0;
            
            // Kopírování konfigurace
            memcpy(&p_fifo_handle->config, ap_fifo_config, sizeof(s_fifo_config_t));

            // Alokování paměti pro buffer
            p_fifo_handle->p_buffer = (uint8_t *) malloc(p_fifo_handle->config.length * p_fifo_handle->config.item_size);
            
            if (NULL != p_fifo_handle->p_buffer) 
            {
                // Alokace bufferu proběhla v pořádku         
                result = 0;
            }
            else 
            {
                // Chyba alokace paměti bufferu
                result = -3;  
            }
        }
        else        
        {
            // Chyba alokace paměti
            result = -3;  
        }
    }

    if (0 == result) 
    {
        // Vse ok - nastavime handle
        // Nastavení ukazatele na FIFO handle
        *app_fifo_handle = p_fifo_handle;
    }
    else
    {
        // Chyba - uklid        
        // Uvolnění alokované paměti
        if (NULL != p_fifo_handle) 
        {
            // Uvolnění bufferu fifo jestliže byl alokován
            if (NULL != p_fifo_handle->p_buffer) 
            {
                // Uvolnění bufferu
                free(p_fifo_handle->p_buffer);
            }
            
            // Uvolnění FIFO handle
            free(p_fifo_handle);
        }
        
        // Nastavení ukazatele na NULL
        *app_fifo_handle = NULL;
    }

    // Navratová hodnota
    return result;
}

// Funkce pro zrušení fifo
int fifo_destroy(s_fifo_handle_t** app_fifo_handle)
{
    int result = 0;
    s_fifo_handle_t *p_fifo_handle = NULL;

    // Kontrola parametru
    if ((NULL == app_fifo_handle) ||
        (NULL == *app_fifo_handle)) 
    {
        // Neplatné parametry
        result = -1;  
    }

    if (0 == result) 
    {
        // Ulozeni si ukazatele pro snadnejsi praci
        p_fifo_handle = *app_fifo_handle;

        // Uvolnění bufferu
        if (NULL != p_fifo_handle->p_buffer) 
        {
            free(p_fifo_handle->p_buffer);
        }
        else 
        {
            // Chyba uvolnění bufferu - neměl by nastat
            result = -4;  
        }
        
        // Uvolnění FIFO handle
        free(p_fifo_handle);

        // Nastavení ukazatele na NULL
        *app_fifo_handle = NULL;
    }

    return result;
}

/*
// Uvolnění FIFO bufferu
void fifo_destroy(FIFO_t* fifo) {
    free(fifo->buffer);  // Uvolnění alokované paměti
}

// Vložení dat do FIFO
int FIFO_push(FIFO_t* fifo, void* buffer) {
    if (FIFO_isFull(fifo)) {
        return -1;  // FIFO je plné
    }

    // Atomičnost
    disable_interrupts();
    // Zkopírování dat do bufferu
    memcpy((uint8_t*)fifo->buffer + fifo->rear * fifo->item_size, buffer, fifo->item_size);
    fifo->rear = (fifo->rear + 1) % fifo->size;
    fifo->count++;
    // Atomičnost
    enable_interrupts();

    return 0;  // Úspěch
}

// Vyjmutí dat z FIFO
int FIFO_pop(FIFO_t* fifo, void* buffer) {
    if (FIFO_isEmpty(fifo)) {
        return -1;  // FIFO je prázdné
    }

    // Atomičnost
    disable_interrupts();
    // Zkopírování dat z bufferu
    memcpy(buffer, (uint8_t*)fifo->buffer + fifo->front * fifo->item_size, fifo->item_size);
    fifo->front = (fifo->front + 1) % fifo->size;
    fifo->count--;
    // Atomičnost
    enable_interrupts();

    return 0;  // Úspěch
}

// Kontrola, jestli je FIFO prázdné
bool FIFO_isEmpty(const FIFO_t* fifo) {
    return fifo->count == 0;
}

// Kontrola, jestli je FIFO plné
bool FIFO_isFull(const FIFO_t* fifo) {
    return fifo->count == fifo->size;
}
*/