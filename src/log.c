#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// Výchozí úroveň logování (může být přepsána voláním `log_set_global_level`)
static log_level_t g_log_global_level = LOG_LEVEL_MAX;
static log_level_t g_log_module_levels[LOG_MODULE_MAX] = { LOG_LEVEL_NONE };

// Výchozí výstupní funkce logování (pokud není nastavena, použije se `printf`)
static fp_log_output_t g_fp_log_output = NULL;

// Převod úrovně logování na string
static const char *log_level_to_string(log_level_t a_level) {
    switch (a_level) {
        case LOG_LEVEL_ERROR: return "ERROR";
        case LOG_LEVEL_WARN:  return "WARN";
        case LOG_LEVEL_INFO:  return "INFO";
        case LOG_LEVEL_DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

// Nastavení globální úrovně logování
void log_set_global_level(log_level_t a_level) {
    g_log_global_level = a_level;
}

// Nastavení úrovně logování pro konkrétní modul
void log_set_module_level(log_module_t a_module, log_level_t a_level) {
    if (a_module < LOG_MODULE_MAX) {
        g_log_module_levels[a_module] = a_level;
    }
}

// Nastavení výstupní funkce logování
void log_set_output_function(fp_log_output_t afp_output) {
    g_fp_log_output = afp_output;
}

// Hlavní funkce pro zpracování logu
void log_message(log_module_t a_module, log_level_t a_level, const char *ap_file, int a_line, const char *ap_fmt, ...) {
    // Kontrola úrovně logování
    if (a_level > g_log_global_level || a_level > g_log_module_levels[a_module]) {
        return;
    }

    // Buffer pro výstupní zprávu
    char buffer[LOG_BUFFER_SIZE];

    // Sestavení zprávy
    int offset = snprintf(buffer, sizeof(buffer), "[%s] %s:%d: ", log_level_to_string(a_level), ap_file, a_line);
    if (offset < 0 || offset >= (int)sizeof(buffer)) {
        return; // Chyba při formátování
    }

    va_list args;
    va_start(args, ap_fmt);
    vsnprintf(buffer + offset, sizeof(buffer) - offset, ap_fmt, args);
    va_end(args);

    // Pokud je nastavena vlastní výstupní funkce, použij ji, jinak `printf`
    if (g_fp_log_output) {
        g_fp_log_output(buffer);
    } else {
        printf("%s\n", buffer);
    }
}
