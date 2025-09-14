#include "xpressnet.h"
#include <stdio.h>
#include <string.h>

XpressnetError_t Xpressnet_Init(XpressnetHandle_t* ap_handle, const XpressnetInitParams_t* ap_params) {
    if (ap_handle == NULL || ap_params == NULL) {
        return XN_ERROR_INVALID_PARAM;  // Chyba - neplatný ukazatel
    }

    ap_handle->status = XN_STATUS_INITIALIZED;
    ap_handle->device_address = ap_params->device_address;

    // Inicializace sériového portu nebo jiné hardware vrstvy podle parametru
    printf("XpressNet Initialized with baud rate: %ld and device address: %lu\n", ap_params->baud_rate, (unsigned long)ap_params->device_address);

    return XN_OK;  // Úspěšně inicializováno
}

XpressnetError_t Xpressnet_Open(XpressnetHandle_t* ap_handle) {
    if (ap_handle == NULL) {
        return XN_ERROR_INVALID_PARAM;  // Chyba - neplatný ukazatel
    }

    if (ap_handle->status != XN_STATUS_INITIALIZED) {
        return XN_ERROR_WRONG_STATE;  // Funkce volána před inicializací
    }

    // Otevření komunikačního kanálu (například USART) podle adresy zařízení
    printf("Opening communication for device with address: %d\n", ap_handle->device_address);

    ap_handle->status = XN_STATUS_OPEN;  // Nastavení stavu na otevřený

    return XN_OK;  // Komunikace otevřena
}

XpressnetError_t Xpressnet_Close(XpressnetHandle_t* ap_handle) {
    if (ap_handle == NULL) {
        return XN_ERROR_INVALID_PARAM;  // Chyba - neplatný ukazatel
    }

    if (ap_handle->status != XN_STATUS_OPEN) {
        return XN_ERROR_WRONG_STATE;  // Funkce volána, když komunikace není otevřená
    }

    // Zavření komunikačního kanálu
    printf("Closing communication for device with address: %d\n", ap_handle->device_address);

    ap_handle->status = XN_STATUS_INITIALIZED;  // Nastavení stavu zpět na inicializovaný

    return XN_OK;  // Komunikace uzavřena
}

XpressnetError_t Xpressnet_SendData(XpressnetHandle_t* ap_handle, const uint8_t* ap_data, uint16_t a_size) {
    if (ap_handle == NULL || ap_data == NULL) {
        return XN_ERROR_INVALID_PARAM;  // Chyba - neplatný ukazatel
    }

    if (ap_handle->status != XN_STATUS_OPEN) {
        return XN_ERROR_WRONG_STATE;  // Funkce volána, když komunikace není otevřená
    }

    // Odeslání dat
    printf("Sending data to device address: %d\n", ap_handle->device_address);
    for (int i = 0; i < a_size; i++) {
        printf("Byte %d: 0x%02X\n", i, ap_data[i]);
    }

    return XN_OK;  // Data odeslána
}

XpressnetError_t Xpressnet_ReceiveData(XpressnetHandle_t* ap_handle, uint8_t* ap_buffer, uint16_t a_size) {
    if (ap_handle == NULL || ap_buffer == NULL) {
        return XN_ERROR_INVALID_PARAM;  // Chyba - neplatný ukazatel
    }

    if (ap_handle->status != XN_STATUS_OPEN) {
        return XN_ERROR_WRONG_STATE;  // Funkce volána, když komunikace není otevřená
    }

    // Příjem dat (pro ilustraci simulováno)
    printf("Receiving data from device address: %d\n", ap_handle->device_address);
    for (int i = 0; i < a_size; i++) {
        ap_buffer[i] = 0xFF;  // Simulace přijetí dat
        printf("Byte %d: 0x%02X\n", i, ap_buffer[i]);
    }

    return XN_OK;  // Data přijata
}

XpressnetError_t Xpressnet_DeInit(XpressnetHandle_t* ap_handle) {
    if (ap_handle == NULL) {
        return XN_ERROR_INVALID_PARAM;  // Chyba - neplatný ukazatel
    }

    // De-inicializace a uvolnění prostředků
    printf("Deinitializing XpressNet for device with address: %d\n", ap_handle->device_address);

    ap_handle->status = XN_STATUS_NOT_INITIALIZED;

    return XN_OK;  // Úspěšně de-inicializováno
}
