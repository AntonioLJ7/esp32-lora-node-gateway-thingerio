#include <Arduino.h>
#include "LoRaWan_APP.h"
#include "conf_wifi.h"
#include "conf_gateway.h"
#include "thinger.h"

void setup() {
  Serial.begin(115200);

  // Inicialización de la placa Heltec (reloj lento para bajo consumo si aplica)
  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

  // Red + Nube + Radio
  iniciarWifi();
  iniciarThinger();
  iniciarLoRa();

  // Entradas físicas del gateway (si las usas como botones HW)
  botonRSTGateway();
  botonRSTNodo();

  // Recursos/parametría de límites y periodo de adquisición
  crearRecursosLimites();
  tiempo_adquisicion_datosBME680();
}

void loop() {
  // Mantener sesión/heartbeat con Thinger.io
  continuarthinger();

  // Procesar interrupciones/IRQ de LoRa y mantener recepción
  mantenerRecepcion();
}