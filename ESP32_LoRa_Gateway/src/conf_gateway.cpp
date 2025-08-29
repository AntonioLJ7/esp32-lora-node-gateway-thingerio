#include "conf_gateway.h"
#include "LoRaWan_APP.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include "thinger.h"

// Variables de sensores recibidos
float temperature = 0.0;
float humidity    = 0.0;
float pressure    = 0.0;
float iaq         = 0.0;

// Flags y temporización
bool alerta_activada   = false;
int intervalo_normal   = 180;   // s
int intervalo_alerta   = 60;    // s

// Eventos LoRa
static RadioEvents_t RadioEvents;

// Variables globales definidas en thinger.cpp (extern aquí)
extern bool   modo_operativo;
extern String modo_actual_nodo;
extern String ultimo_modo_enviado;

void iniciarLoRa(void) {
  Serial.println(F("\n🚀 Iniciando LoRa..."));

  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxDone = OnTxDone;

  Radio.Init(&RadioEvents);
  Radio.SetChannel(RF_FREQUENCY);

  Radio.SetRxConfig(MODEM_LORA,
                    LORA_BANDWIDTH,
                    LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE,
                    0,
                    LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT,
                    LORA_FIX_LENGTH_PAYLOAD_ON,
                    0,
                    true, 0, 0,
                    LORA_IQ_INVERSION_ON,
                    true);

  Serial.println(F("📡 Gateway en modo RX (escuchando paquetes)"));
  Radio.Rx(0);
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
  Serial.println(F("\n📥 Paquete recibido!"));

  // Copiar payload a string seguro
  char rxpacket[size + 1];
  memcpy(rxpacket, payload, size);
  rxpacket[size] = '\0';
  String msg = String(rxpacket);

  Serial.printf("📨 Mensaje recibido: \"%s\", RSSI=%d, Tamaño=%d, SNR=%d\n",
                msg.c_str(), rssi, size, snr);

  // 🚨 Si es alerta directa
  if (msg.startsWith("ALERTA:")) {
    Serial.println(F("🚨 Alerta recibida desde el nodo"));
    procesarAlerta(msg);
    Radio.Rx(0);
    return;
  }

  // Intentar parsear como JSON
  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, msg);
  if (error) {
    Serial.printf("❌ Error al parsear JSON: %s\n", error.c_str());
    Radio.Rx(0);
    return;
  }

  // Extraer valores
  temperature = doc["temp"] | 0.0;
  humidity    = doc["hum"]  | 0.0;
  pressure    = doc["pres"] | 0.0;
  iaq         = doc["iaq"]  | 0.0;

  Serial.printf("🌡️ Temp: %.2f°C, 💧 Hum: %.2f%%, ⬇️ Presión: %.2f hPa, 🧪 IAQ: %.2f\n",
                temperature, humidity, pressure, iaq);

  // Analizar alerta en función de límites configurados en thinger
  bool en_alerta = (temperature > t_max || humidity > h_max || iaq > g_max);

  if (en_alerta) {
    if (!alerta_activada) {
      alerta_activada = true;
      String comando = "SET:intervalo=" + String(intervalo_alerta) + ";";
      Radio.Send((uint8_t *)comando.c_str(), comando.length());
      Serial.println(F("⚙️ Intervalo reducido por alerta"));
      delay(100);
    }

    String alerta = "!ALERTA¡ :";
    if (temperature > t_max) alerta += " TEMPERATURA=" + String(temperature, 1);
    if (humidity > h_max)    alerta += " HUMEDAD=" + String(humidity, 1);
    if (iaq > g_max)         alerta += " IAQ=" + String(iaq, 1);
    procesarAlerta(alerta);

  } else if (alerta_activada) {
    alerta_activada = false;
    procesarAlerta("Sin alertas");

    String comando = "SET:intervalo=" + String(intervalo_normal) + ";";
    Radio.Send((uint8_t *)comando.c_str(), comando.length());
    Serial.println(F("✅ Valores normales, intervalo restaurado"));
    delay(100);
  }

  // Actualizar datos en Thinger.io
  definirRecursosThinger(temperature, humidity, pressure, iaq);

  // Verificar y enviar modo solo si ha cambiado
  if (modo_actual_nodo != ultimo_modo_enviado) {
    Serial.printf("🔁 Nodo activo. Reenviando modo: %s\n", modo_actual_nodo.c_str());
    Radio.Send((uint8_t *)modo_actual_nodo.c_str(), modo_actual_nodo.length());
    ultimo_modo_enviado = modo_actual_nodo;
    delay(100);
  } else {
    Serial.println(F("✔️ El nodo ya tiene el modo correcto."));
  }

  Radio.Rx(0);
}

void OnTxDone() {
  Serial.println(F("✅ TX finalizado"));
}

void mantenerRecepcion(void) {
  Radio.IrqProcess();
}