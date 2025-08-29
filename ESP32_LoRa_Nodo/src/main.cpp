#include <Arduino.h>
#include "LoRaWan_APP.h"
#include "lcd.h"
#include "bme_680.h"
#include "Conf_LoRa.h"
#include <Wire.h>
#include <Preferences.h>
#include <esp_sleep.h>

extern int tiempo_muestreo;
extern bool lora_idle;
extern String ordenModo;

Preferences prefs;
bool modo_config = true;

float tem = 0.0, hum = 0.0, pres = 0.0, iaq = 0.0;
unsigned long ultimo_envio = 0;

void setup() {
  Serial.begin(115200);

  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
  Wire.begin(7, 6);
  iniciarLoRa();

  if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TIMER) {
    modo_config = true;
    prefs.begin("config", false);
    prefs.putBool("modo_config", true);
    prefs.end();
  } else {
    prefs.begin("config", true);
    modo_config = prefs.getBool("modo_config", true);
    prefs.end();
  }

  if (!iniciarBME680()) {
    Serial.println("❌ No se encontró el sensor BME680.");
  } else {
    Serial.println("✅ Sensor BME680 inicializado correctamente.");
  }

  if (modo_config) {
    iniciarLCD();
    mostrarModoLCD("Modo Configuracion");
  }

  toma_datos_BME680(tem, hum, pres, iaq);
  if (modo_config) visualizarDatos_BME680_LCD(tem, hum, pres, iaq);
  envioPaquetes(tem, hum, pres, iaq);

  if (modo_config) {
    ultimo_envio = millis();
  }

  if (!modo_config) {
    unsigned long t0 = millis();
    Serial.println("⏳ Esperando 10s por si Gateway responde...");
    while (millis() - t0 < 10000) {
      Radio.IrqProcess();
      delay(10);
    }

    Serial.println("😴 Entrando en deep sleep...");
    esp_sleep_enable_timer_wakeup(tiempo_muestreo * 1000000ULL);
    esp_deep_sleep_start();
  }
}

void loop() {
  if (ordenModo == "MODO=OPERATIVO") {
    modo_config = false;
    prefs.begin("config", false);
    prefs.putBool("modo_config", false);
    prefs.end();

    mostrarModoLCD("Modo Operativo");
    Serial.println("🟩 Cambiado a modo operativo");
    ordenModo = "";

    delay(3000);
    apagarLCD();

    toma_datos_BME680(tem, hum, pres, iaq);
    envioPaquetes(tem, hum, pres, iaq);

    unsigned long t0 = millis();
    Serial.println("⏳ Esperando 10s por si Gateway responde...");
    while (millis() - t0 < 10000) {
      Radio.IrqProcess();
      delay(10);
    }

    Serial.println("😴 Entrando en deep sleep...");
    esp_sleep_enable_timer_wakeup(tiempo_muestreo * 1000000ULL);
    esp_deep_sleep_start();
  }

  if (ordenModo == "MODO=CONFIG") {
    modo_config = true;
    prefs.begin("config", false);
    prefs.putBool("modo_config", true);
    prefs.end();

    mostrarModoLCD("Modo Configuracion");
    Serial.println("🟦 Modo cambiado a configuración");
    ordenModo = "";
    delay(1000);
    ESP.restart();
  }

  if (modo_config) {
    unsigned long ahora = millis();
    if (ahora - ultimo_envio >= tiempo_muestreo * 1000UL && lora_idle) {
      ultimo_envio = ahora;
      toma_datos_BME680(tem, hum, pres, iaq);
      visualizarDatos_BME680_LCD(tem, hum, pres, iaq);
      envioPaquetes(tem, hum, pres, iaq);
    }
  }

  Radio.IrqProcess();
}