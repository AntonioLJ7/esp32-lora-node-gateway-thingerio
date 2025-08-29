#ifndef BME_680_H
#define BME_680_H

#include <Arduino.h>

bool iniciarBME680();
void toma_datos_BME680(float &tem, float &hum, float &pres, float &iaq);

#endif