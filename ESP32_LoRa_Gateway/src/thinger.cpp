#include "thinger.h"
#include <Arduino.h>
#include <ThingerESP32.h>
#include "LoRaWan_APP.h"
#include "conf_gateway.h"
#include "conf_wifi.h"

// ================== Credenciales Thinger.io (definidas en thinger.h) ==================
ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);


static const char* SSID     = "";
static const char* PASSWORD = "";

// ================== Estado / variables compartidas ==================
String mensaje_alerta = "Sin alertas";

bool   modo_operativo       = false;
bool   lora_idle            = true;
bool   modo_pendiente_envio = false;   
int    periodo_actual       = 60;

String modo_actual_nodo     = "MODO=CONFIG";
String ultimo_modo_enviado  = "";

// Límites configurables desde Thinger
float t_max = 42.0f;
float h_max = 90.0f;
float g_max = 200.0f;

// ================== Inicialización de Thinger.io ==================
void iniciarThinger(void){

  if (SSID[0] != '\0') {
    thing.add_wifi(SSID, PASSWORD);
  }

  // Recursos de medición (solo lectura)
  thing["Temperatura"] >> [](pson &out){ out = 0.0; }; 
  thing["Humedad"]     >> [](pson &out){ out = 0.0; };
  thing["Presion"]     >> [](pson &out){ out = 0.0; };
  thing["IAQ"]         >> [](pson &out){ out = 0.0; };

  // Alerta (solo lectura desde dashboard; se actualiza desde procesarAlerta)
  thing["alerta_txt"] >> [](pson &out){
    out = mensaje_alerta;
  };

  // Modo operativo (lectura/escritura)
  thing["modo_operativo"] << [](pson &in){
    if (in.is_empty()) {
      in = modo_operativo;
    } else {
      modo_operativo   = (bool) in;
      modo_actual_nodo = modo_operativo ? "MODO=OPERATIVO" : "MODO=CONFIG";
      Serial.printf("🔄 Modo cambiado desde Thinger.io → %s (se enviará al nodo en la próxima oportunidad)\n",
                    modo_actual_nodo.c_str());
    }
  };

  // Límites editables
  thing["t_max"] << [](pson &in){
    if (in.is_empty()) in = t_max;
    else { t_max = (float) in; Serial.printf("🌡️ Nuevo t_max: %.2f\n", t_max); }
  };
  thing["h_max"] << [](pson &in){
    if (in.is_empty()) in = h_max;
    else { h_max = (float) in; Serial.printf("💧 Nuevo h_max: %.2f\n", h_max); }
  };
  thing["g_max"] << [](pson &in){
    if (in.is_empty()) in = g_max;
    else { g_max = (float) in; Serial.printf("🫧 Nuevo iaq_max: %.2f\n", g_max); }
  };

  // Botón REST del Gateway
  thing["RST_Gateway"] << [] (pson& activated){
    if((bool)activated){
      ESP.restart();
    }
  };

  // Botón RESET del Nodo (vía LoRa)
  thing["RST_Nodo"] << [] (pson& activated){
    if((bool)activated){
      Serial.println("🔁 Enviando comando RST al nodo...");
      const char* rstMessage = "RST";
      Radio.Send((uint8_t *)rstMessage, strlen(rstMessage));
      delay(1000);
      Radio.Rx(0);
    }
  };

  // Intervalo editable → envía SET:intervalo=<seg>;
  thing["tiempoBME"] << [](pson &in) {
    if (in.is_empty()) {
      in = periodo_actual;
    } else {
      int nuevo_periodo = (int) in;
      if (nuevo_periodo >= 10) {
        periodo_actual = nuevo_periodo;
        Serial.printf("🕒 Nuevo periodo de muestreo: %d s\n", periodo_actual);

        String msg = "SET:intervalo=" + String(periodo_actual) + ";";
        Radio.Send((uint8_t *)msg.c_str(), msg.length());
        delay(100);
        Radio.Rx(0);
      } else {
        Serial.println("⚠️ Periodo demasiado bajo. Mínimo permitido: 10 s.");
      }
    }
  };
}

// Actualiza los recursos de medidas con los últimos valores recibidos del nodo
void definirRecursosThinger(float t, float h, float p, float i){
  thing["Temperatura"] >> [=](pson &out){ out = t; };
  thing["Humedad"]     >> [=](pson &out){ out = h; };
  thing["Presion"]     >> [=](pson &out){ out = p; };
  thing["IAQ"]         >> [=](pson &out){ out = i; };
}

// Notifica una alerta (texto + endpoint opcional llamado "enviar_alerta")
void procesarAlerta(const String& msg) {
  mensaje_alerta = msg;
  thing.call_endpoint("enviar_alerta");
}

// Recursos ya creados en iniciarThinger (mantenemos firmas por compatibilidad)
void crearRecursosLimites(void) {}
void tiempo_adquisicion_datosBME680(void) {}

// Loop de mantenimiento de Thinger
void continuarthinger(void){
  thing.handle();
}