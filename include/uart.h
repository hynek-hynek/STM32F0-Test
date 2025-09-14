#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "uart_shared.h"
#include "fifo.h"

/* Definice maximálních velikostí bufferů (typu uart_data_t) */
#define UART_BUFFER_LENGTH_RX 128
#define UART_BUFFER_LENGTH_TX 128

/* Definice stavů UARTu */
typedef enum e_uart_status {
    e_UART_STATUS_NOT_INITIALIZED,
    e_UART_STATUS_INITIALIZED,
    e_UART_STATUS_OPEN
} e_uart_status_t;

/* Konfigurace UARTu pro init */
typedef struct s_uart_config {
    uint16_t rx_buffer_length;  // Delka prijimaciho bufferu
    uint16_t tx_buffer_length;  // Delka vysilaciho bufferu
    void *p_platform_handle;    // Platformově závislá konfigurace – její obsah se neověřuje
    e_uart_result_t (*fp_platform_create)(void *);
    e_uart_result_t (*fp_platform_destroy)(void *);
    e_uart_result_t (*fp_platform_open)(void *);
    e_uart_result_t (*fp_platform_close)(void *);
    e_uart_result_t (*fp_platform_send)(void *, const uint16_t *, size_t);
    e_uart_result_t (*fp_platform_receive)(void *, uint16_t *, size_t, size_t *);
} s_uart_config_t;

/* Typ pro callback funkce, která zpracuje jeden  znak */
typedef void (*uart_char_callback_t)(uart_data_t aChar);

/* Ovladač UARTu */
typedef struct s_uarthandle {
    s_fifo_handle_t *p_rx_fifo;
    s_fifo_handle_t *p_tx_fifo;
    s_uart_config_t config;
    e_uart_status_t status;
} s_uart_handle_t;

/* Prototypy funkcí UART driveru */
e_uart_result_t uart_create(s_uart_handle_t **ap_handle, const s_uart_config_t *ap_config);
e_uart_result_t uart_destroy(s_uart_handle_t **ap_handle);
e_uart_result_t uart_open(s_uart_handle_t *ap_handle);
e_uart_result_t uart_close(s_uart_handle_t *ap_handle);
e_uart_result_t uart_send(s_uart_handle_t *ap_handle, const uint16_t *ap_data, size_t a_length);
e_uart_result_t uart_receive(s_uart_handle_t *ap_handle, uint16_t *ap_buffer, size_t a_buffer_size, size_t *ap_received_length);

#ifdef __cplusplus
}
#endif

#endif /* UART_H */
