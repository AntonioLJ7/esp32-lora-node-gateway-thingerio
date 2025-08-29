#ifndef _CONF_LORA_H_
#define _CONF_LORA_H_

#include <Arduino.h>
#include <stdint.h>
#include <Preferences.h>

// --- Parámetros de radio ---
#define RF_FREQUENCY                868000000 // Hz
#define TX_OUTPUT_POWER             14        // dBm
#define LORA_BANDWIDTH              0         // [0:125kHz, 1:250kHz, 2:500kHz]
#define LORA_SPREADING_FACTOR       7         // [SF7..SF12]
#define LORA_CODINGRATE             1         // [1:4/5, 2:4/6, 3:4/7, 4:4/8]
#define LORA_PREAMBLE_LENGTH        8
#define LORA_SYMBOL_TIMEOUT         0
#define LORA_FIX_LENGTH_PAYLOAD_ON  false
#define LORA_IQ_INVERSION_ON        false

#define RX_TIMEOUT_VALUE            1000
#define BUFFER_SIZE                 80  // tamaño máx. de payload

// --- API ---
void iniciarLoRa(void);
void envioPaquetes(float tem, float hum, float pres, float iaq);

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
void OnTxDone(void);
void OnTxTimeout(void);

// --- Variables compartidas ---
extern int tiempo_muestreo;
extern bool lora_idle;
extern String ordenModo;
extern Preferences prefs;

#endif