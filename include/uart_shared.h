#ifndef UART_SHARED_H
#define UART_SHARED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Definice výsledkových kódů UARTu */
typedef enum e_uart_result {
    e_UART_RESULT_OK,
    e_UART_RESULT_ERROR,
    e_UART_RESULT_MEMORY_ERROR,
    e_UART_RESULT_INVALID_PARAM,
    e_UART_RESULT_WRONG_STATE,
    e_UART_RESULT_FATAL_ERROR,
} e_uart_result_t;

/* Definice typu znaku UARTu */
typedef uint16_t uart_data_t;

#ifdef __cplusplus
}
#endif

#endif // UART_SHARED_H