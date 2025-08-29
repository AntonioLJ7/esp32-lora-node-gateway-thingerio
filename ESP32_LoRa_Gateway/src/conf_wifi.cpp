#include "conf_wifi.h"
#include <WiFi.h>

// Rellena tus credenciales aquí (o cárgalas desde Preferences si lo prefieres)
static const char* ssid     = "";   // <-- TU SSID
static const char* password = "";   // <-- TU PASSWORD

void iniciarWifi(void){
  WiFi.mode(WIFI_STA);
  Serial.printf("Conectando a WiFi \"%s\" ", ssid && ssid[0] ? ssid : "(ssid vacío)");

  WiFi.begin(ssid, password);

  // Espera hasta 20 s como máximo
  const uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - t0) < 20000UL) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\n✅ Conectado. IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\n❌ No se pudo conectar al WiFi (timeout). Revisa credenciales o cobertura.");
  }
}