# IoT Motor Health Guard

An end-to-end ESP32 project that watches motor current and vibration, alarms locally, and publishes live MQTT telemetry.

## Features

- ACS712 RMS-current measurement and MPU6050 vibration monitoring
- LED and buzzer alarm with configurable health limits
- JSON telemetry over MQTT
- Browser dashboard in `dashboard/index.html`
- Python MQTT simulator in `simulator/simulate.py`

## Hardware

- ESP32 development board
- MPU6050 accelerometer/gyroscope (I2C)
- ACS712 current sensor (use an appropriately rated model)
- LED and optional active buzzer

| Device | ESP32 connection |
| --- | --- |
| MPU6050 VCC/GND | 3.3V/GND |
| MPU6050 SDA/SCL | GPIO 21/GPIO 22 |
| ACS712 OUT | GPIO 34 |
| LED | GPIO 2 |
| Active buzzer | GPIO 25 |

## Setup

1. In Arduino IDE, install the **ESP32 by Espressif** board package.
2. Install `Adafruit MPU6050`, `Adafruit Unified Sensor`, and `PubSubClient` from Library Manager.
3. Open `IoT_Motor_Health_Guard.ino`.
4. Set your Wi-Fi name/password and MQTT broker details.
5. With the motor powered but switched off, calibrate `CURRENT_ZERO_VOLTAGE` using the observed ACS712 output.
6. Tune `MAX_RMS_CURRENT_A` and `MAX_VIBRATION_MS2` after measuring the motor in a known-good state.

## Dashboard and simulator

Open `dashboard/index.html` in a browser, enter a WebSocket-enabled MQTT broker and the topic, then connect. To test without hardware, install Python 3 plus `paho-mqtt` and run:

```bash
pip install paho-mqtt
python simulator/simulate.py --host broker.hivemq.com --topic iot-motor-health-guard/telemetry
```

The public broker in the sample is for experimentation only. Use an authenticated private broker for a real deployment.

Detailed wiring and calibration instructions are in [docs/hardware.md](docs/hardware.md).

The device posts JSON every five seconds, for example:

```json
{"current_a":1.42,"vibration_ms2":10.18,"overload":false,"excessive_vibration":false,"status":"HEALTHY"}
```

## Safety

The ACS712 must be installed according to its datasheet and your motor's voltage/current rating. Do not connect mains voltage to the ESP32 or breadboard. Use proper isolation and seek qualified supervision for mains-powered motors.

