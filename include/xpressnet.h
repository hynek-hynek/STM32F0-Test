#ifndef XPRESSNET_H
#define XPRESSNET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Enum pro stav XpressNet komunikace
typedef enum {
    XN_STATUS_NOT_INITIALIZED = 0, // Komunikace není inicializována
    XN_STATUS_INITIALIZED,          // Komunikace inicializována
    XN_STATUS_OPEN,                 // Komunikace je otevřena
    XN_STATUS_WRONG_STATE          // Chybný stav (např. pokus o volání funkce ve špatném stavu)
} XpressnetStatus_t;

// Enum pro návratové kódy funkcí
typedef enum {
    XN_OK = 0,                    // Úspěch
    XN_ERROR_INVALID_PARAM = -1,   // Neplatný parametr
    XN_ERROR_WRONG_STATE = -2,     // Chybný stav (např. volání funkce ve špatném stavu)
    XN_ERROR_COMMUNICATION = -3    // Chyba komunikace
} XpressnetError_t;

// Struktura pro inicializaci
typedef struct {
    uint32_t baud_rate;     // Baud rate pro komunikaci
    uint8_t device_address;  // Adresa zařízení na XpressNet
} XpressnetInitParams_t;

// Struktura pro XpressNet "handle"
typedef struct {
    XpressnetStatus_t status;          // Stav XpressNet komunikace
    uint8_t device_address;            // Adresa zařízení
} XpressnetHandle_t;

// Funkce pro inicializaci XpressNet
XpressnetError_t Xpressnet_Init(XpressnetHandle_t* ap_handle, const XpressnetInitParams_t* ap_params);

// Funkce pro otevření komunikace (Master nebo Slave režim)
XpressnetError_t Xpressnet_Open(XpressnetHandle_t* ap_handle);

// Funkce pro uzavření komunikace
XpressnetError_t Xpressnet_Close(XpressnetHandle_t* ap_handle);

// Funkce pro odeslání dat
XpressnetError_t Xpressnet_SendData(XpressnetHandle_t* ap_handle, const uint8_t* ap_data, uint16_t a_size);

// Funkce pro přijetí dat
XpressnetError_t Xpressnet_ReceiveData(XpressnetHandle_t* ap_handle, uint8_t* ap_buffer, uint16_t a_size);

// Funkce pro deinitializaci XpressNet
XpressnetError_t Xpressnet_DeInit(XpressnetHandle_t* ap_handle);

#ifdef __cplusplus
}
#endif

#endif // XPRESSNET_H
    