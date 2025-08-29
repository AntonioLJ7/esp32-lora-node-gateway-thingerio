#ifndef _THINGER_H_
#define _THINGER_H_

#include <Arduino.h>

// 📡 Configuración de Thinger.io
#define USERNAME ""            // ⬅️ Tu usuario de Thinger.io
#define DEVICE_ID ""           // ⬅️ Device ID del Gateway
#define DEVICE_CREDENTIAL ""   // ⬅️ Device Credentials del Gateway

// Funciones principales
void iniciarThinger(void);
void continuarthinger(void);
void definirRecursosThinger(float t, float h, float p, float i);
void crearRecursosLimites(void);
void tiempo_adquisicion_datosBME680(void);
void procesarAlerta(const String& msg);

// Funciones auxiliares (botones físicos si los usas)
void botonRSTNodo(void);
void botonRSTGateway(void);

// Variables globales compartidas
extern bool lora_idle;
extern bool modo_pendiente_envio;
extern bool modo_operativo;

extern String ultimo_modo_enviado;
extern String modo_actual_nodo;

extern float t_max;
extern float h_max;
extern float g_max;

#endif // _THINGER_H_