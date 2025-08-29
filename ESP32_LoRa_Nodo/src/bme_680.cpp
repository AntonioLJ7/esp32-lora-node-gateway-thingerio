#include <Wire.h>
#include <bsec.h>
#include "bme_680.h"

static Bsec iaqSensor;

bool iniciarBME680() {
  Wire.begin();

  if (!iaqSensor.begin(BME68X_I2C_ADDR_HIGH, Wire)) {
    Serial.println(F("❌ Error iniciando BME680 con BSEC"));
    return false;
  }

  bsec_virtual_sensor_t sensorList[4] = {
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_IAQ
  };

  iaqSensor.updateSubscription(sensorList, 4, BSEC_SAMPLE_RATE_LP);
  return true;
}

void toma_datos_BME680(float &tem, float &hum, float &pres, float &iaq) {
  if (iaqSensor.run()) {
    tem  = iaqSensor.temperature;
    hum  = iaqSensor.humidity;
    pres = iaqSensor.pressure / 100.0f;
    iaq  = iaqSensor.iaq;
  } else {
    Serial.println(F("⚠️ Error en la lectura del BME680 (BSEC)"));
  }
}