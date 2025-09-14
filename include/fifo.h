#ifndef FIFO_H
#define FIFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>  // Použití stdint.h pro specifikované velikosti typů

// Maximální velikost FIFO bufferu (65 536 položek)
#define FIFO_MAX_LENGTH 65536
// Maximální velikost FIFO položky (256 velikost)
#define FIFO_MAX_ITEM_SIZE 256


// Struktura pro FIFO buffer
typedef struct s_fifo_config {
    uint32_t length;            // Délka FIFO (počet položek, maximálně 65535)
    uint16_t item_size;         // Velikost položky
    bool is_irq_safe;           // Zda je FIFO bezpečné pro přerušení
} s_fifo_config_t;


// Struktura pro FIFO buffer
typedef struct s_fifo_handle {
    uint8_t* p_buffer;           // Ukazatel na buffer pro data
    volatile uint32_t front;    // Ukazatel na přední prvek
    volatile uint32_t rear;     // Ukazatel na zadní prvek
    volatile uint32_t count;    // Počet prvků v FIFO
    s_fifo_config_t config;     // Konfigurace FIFO
} s_fifo_handle_t;

// Funkce pro vytvoření fifo
int fifo_create(s_fifo_handle_t** app_fifo, s_fifo_config_t* ap_fifo_config);
// Funkce pro zrušení fifo
int fifo_destroy(s_fifo_handle_t** app_fifo_handle);



/*

// Funkce pro vložení dat do FIFO
int FIFO_push(FIFO_t* fifo, void* buffer);

// Funkce pro vyjmutí dat z FIFO
int FIFO_pop(FIFO_t* fifo, void* buffer);

// Kontrola, jestli je FIFO prázdné
bool FIFO_isEmpty(const FIFO_t* fifo);

// Kontrola, jestli je FIFO plné
bool FIFO_isFull(const FIFO_t* fifo);
*/
#ifdef __cplusplus
}
#endif

#endif // FIFO_H