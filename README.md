Environmental Monitoring System with ESP32 and LoRa

This project implements an environmental monitoring system based on two Heltec ESP32 Wireless Stick Lite V3 (SX1262) boards that communicate over LoRa.

Node: acquires environmental data from a BME680 sensor (temperature, humidity, pressure, air quality), displays them on an LCD 16x4 screen, and sends them via LoRa.

Gateway: receives the data, processes it, and forwards it to Thinger.io for storage, dashboard visualization, and configurable alerts.

✨ Features

LoRa point-to-point communication (868 MHz).

Environmental sensing with BME680.

Local feedback via LCD 16x4 (I²C).

Thinger.io integration: dashboards, resources, and notifications (email, Telegram, etc.).

Two operation modes:

Configuration mode: the node remains active and allows real-time parameter adjustments.

Operational mode (low power): the node enters deep sleep and wakes up at defined intervals.

Remote configurable parameters:

Measurement interval.

Maximum temperature threshold.

Maximum humidity threshold.

Minimum air quality (gas) threshold.

🧰 Hardware

2 × Heltec Wireless Stick Lite V3 (ESP32 + SX1262).

Adafruit BME680 sensor (I²C).

LCD 16x4 with I²C backpack.

Wires, USB or battery power supply.

🗂️ Repository Structure
.
├─ node/         # Node source code
│  └─ src/
├─ gateway/      # Gateway source code
│  └─ src/
├─ docs/         # Diagrams and dashboard screenshots
└─ README.md

⚙️ Installation
Option A: Arduino IDE

Install ESP32 board support.

Add required libraries:

Heltec LoRa

Adafruit BME680

LiquidCrystal_I2C

Thinger.io client

Open and upload node/src/main.cpp and gateway/src/main.cpp.

Option B: PlatformIO (recommended)

Install VS Code + PlatformIO.

Open the node/ folder and build/upload.

Repeat for the gateway/ folder.

📡 How it Works

The node reads sensor data → displays them on LCD → sends via LoRa.

The gateway receives → stores configuration parameters → sends data and alerts to Thinger.io.

Thresholds and operating mode can be updated remotely through the Thinger.io dashboard.

🚨 Alerts

The system generates alerts when:

temperature > t_max

humidity > h_max

gas < g_min

Alerts are shown in the dashboard (alerta_txt) and can trigger Thinger.io endpoints (e.g., email or Telegram notifications).

📜 License

This project is released under the MIT License.
