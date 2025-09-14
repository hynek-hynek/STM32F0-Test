#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define LOG_BUFFER_SIZE 128

// Definice úrovní logování
typedef enum {
    LOG_LEVEL_NONE = 0,  // Žádné logování
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG, LOG_LEVEL_MAX
} log_level_t;

// Definice modulů logování
typedef enum {
    LOG_MODULE_MAIN = 0,
    LOG_MODULE_UART,
    LOG_MODULE_MAX
} log_module_t;

// Definice ukazatele na funkci pro výstup logování
typedef void (*fp_log_output_t)(const char *ap_message);

// Makro pro logování, odstraní volání, pokud je globální level LOG_LEVEL_NONE
#if defined(ENABLE_LOGGING) && (ENABLE_LOGGING > LOG_LEVEL_NONE)
    #define LOG(a_module, a_level, a_fmt, ...) \
        log_message(a_module, a_level, __FILE__, __LINE__, a_fmt, ##__VA_ARGS__)
#else
    #define LOG(a_module, a_level, a_fmt, ...)  // Logování je vypnuté, kód se negeneruje
#endif

// Deklarace funkcí
void log_message(log_module_t a_module, log_level_t a_level, const char *ap_file, int a_line, const char *ap_fmt, ...);
void log_set_global_level(log_level_t a_level);
void log_set_module_level(log_module_t a_module, log_level_t a_level);
void log_set_output_function(fp_log_output_t afp_output);

#ifdef __cplusplus
}
#endif

#endif // LOG_H
