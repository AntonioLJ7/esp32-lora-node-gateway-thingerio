#ifndef _CONF_GATEWAY_H_
#define _CONF_GATEWAY_H_

#include <Arduino.h>   // para String, uint*_t, etc.
#include <stdint.h>

// ========= Configuración LoRa (EU 868 MHz) =========
#define RF_FREQUENCY                868000000UL  // 868 MHz (cambia a 915E6 si América)
#define TX_OUTPUT_POWER             14           // dBm

// [0:125 kHz, 1:250 kHz, 2:500 kHz]
#define LORA_BANDWIDTH              0
// [SF7..SF12]
#define LORA_SPREADING_FACTOR       7
// [1:4/5, 2:4/6, 3:4/7, 4:4/8]
#define LORA_CODINGRATE             1

#define LORA_PREAMBLE_LENGTH        8
#define LORA_SYMBOL_TIMEOUT         0
#define LORA_FIX_LENGTH_PAYLOAD_ON  false
#define LORA_IQ_INVERSION_ON        false

#ifndef BUFFER_SIZE
#define BUFFER_SIZE                 128   // tamaño de RX/TX del gateway
#endif

// ========= API =========
void iniciarLoRa(void);
void mantenerRecepcion(void);
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
void OnTxDone(void);

#endif // _CONF_GATEWAY_H_