#ifndef LCD_H
#define LCD_H

#include <Arduino.h>

void iniciarLCD();
void mostrarModoLCD(const String& texto);
void visualizarDatos_BME680_LCD(float tem, float hum, float pres, float iaq);
void apagarLCD();

#endif