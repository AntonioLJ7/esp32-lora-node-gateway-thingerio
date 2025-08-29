#include "Conf_LoRa.h"
#include "LoRaWan_APP.h"
#include <Arduino.h>
#include <Preferences.h>

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];

bool lora_idle = true;
String ordenModo = "";
int tiempo_muestreo = 60;

static RadioEvents_t RadioEvents;

// prefs lo define main.cpp
extern Preferences prefs;

void iniciarLoRa() {
  RadioEvents.TxDone    = OnTxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxDone    = OnRxDone;

  Radio.Init(&RadioEvents);
  Radio.SetChannel(RF_FREQUENCY);

  Radio.SetTxConfig(
      MODEM_LORA,
      TX_OUTPUT_POWER,
      0,
      LORA_BANDWIDTH,
      LORA_SPREADING_FACTOR,
      LORA_CODINGRATE,
      LORA_PREAMBLE_LENGTH,
      LORA_FIX_LENGTH_PAYLOAD_ON,
      true, 0, 0,
      LORA_IQ_INVERSION_ON,
      3000
  );

  Radio.Rx(0);
}

void envioPaquetes(float tem, float hum, float pres, float iaq) {
  if (!lora_idle) return;

  // 🔒 snprintf evita desbordes
  snprintf(txpacket, sizeof(txpacket),
           "{\"temp\":%.2f,\"hum\":%.2f,\"pres\":%.2f,\"iaq\":%.2f}",
           tem, hum, pres, iaq);

  Serial.printf("⏳ Enviando paquete: %s\n", txpacket);
  Radio.Send(reinterpret_cast<uint8_t*>(txpacket), strlen(txpacket));
  lora_idle = false;
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
  size = (size < BUFFER_SIZE-1) ? size : BUFFER_SIZE-1;
  memcpy(rxpacket, payload, size);
  rxpacket[size] = '\0';

  String msg(rxpacket);
  Serial.printf("📥 Recibido: %s\n", msg.c_str());

  if (msg == "MODO=CONFIG") {
    Serial.println("🔁 Cambio de modo a CONFIG recibido. Reiniciando...");
    prefs.begin("config", false);
    prefs.putBool("modo_config", true);
    prefs.end();
    delay(300);
    ESP.restart();
    return;
  }

  if (msg == "MODO=OPERATIVO") {
    ordenModo = msg;
    lora_idle = true;
    Radio.Rx(0);
    return;
  }

  if (msg == "RST") {
    Serial.println("🔁 Comando RST recibido. Reiniciando...");
    delay(100);
    ESP.restart();
  }

  if (msg.startsWith("SET:")) {
    msg.remove(0, 4);
    int i1 = msg.indexOf("intervalo=");
    if (i1 >= 0) {
      int fin = msg.indexOf(";", i1);
      if (fin < 0) fin = msg.length();
      int nuevo_intervalo = msg.substring(i1 + 10, fin).toInt();
      if (nuevo_intervalo >= 10) {
        tiempo_muestreo = nuevo_intervalo;
        Serial.printf("🕒 Tiempo de muestreo actualizado a %d segundos\n", tiempo_muestreo);
      } else {
        Serial.println("⚠️ Tiempo de muestreo rechazado: debe ser ≥ 10s");
      }
    }
  }

  lora_idle = true;
  Radio.Rx(0);
}

void OnTxDone(void) {
  Serial.println("✅ TX finalizado correctamente");
  lora_idle = true;
  Serial.println("👂 Activando modo recepción LoRa...");
  Radio.Rx(0);
}

void OnTxTimeout(void) {
  Serial.println("⚠️ Timeout en envío LoRa");
  lora_idle = true;
  Radio.Rx(0);
}