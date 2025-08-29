#include "lcd.h"
#include <LiquidCrystal_I2C.h>
#include "bme_680.h"

static LiquidCrystal_I2C lcd(0x27, 16, 4);

void iniciarLCD() {
  lcd.init();
  lcd.begin(16, 4);
  lcd.backlight();

  lcd.setCursor(0, 0); lcd.print("Monitor systems v1.0 ");
  lcd.setCursor(0, 1); lcd.print("ESP32 + LoRa");
  lcd.setCursor(0, 2); lcd.print("Univ. Malaga [TFG]");
  lcd.setCursor(0, 3); lcd.print("Version 1.0");

  delay(5000);
  lcd.clear();
}

void visualizarDatos_BME680_LCD(float tem, float hum, float pres, float iaq) {
  lcd.setCursor(0, 0); lcd.print("Temp: "); lcd.print(tem); lcd.print(" C ");
  lcd.setCursor(0, 1); lcd.print("Humedad: "); lcd.print(hum); lcd.print(" %");
  lcd.setCursor(0, 2); lcd.print("Presion: "); lcd.print(pres); lcd.print(" hPa");
  lcd.setCursor(0, 3); lcd.print("IAQ: "); lcd.print(iaq);
}

void mostrarModoLCD(const String& texto) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(texto);
  delay(3000);
  lcd.clear();
}

void apagarLCD() {
  lcd.clear();
  lcd.noBacklight();
}