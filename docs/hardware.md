# Hardware and calibration

| Component | ESP32 connection | Note |
| --- | --- | --- |
| MPU6050 VCC/GND | 3.3V/GND | Use 3.3 V logic. |
| MPU6050 SDA/SCL | GPIO 21/GPIO 22 | I2C. |
| ACS712 OUT | GPIO 34 | Never exceed 3.3 V ADC input. |
| LED | GPIO 2 via resistor | Cathode to GND. |
| Active buzzer | GPIO 25 | Use a transistor driver for higher-current buzzers. |

With the motor circuit de-energized, measure the ACS712 no-current output and set `CURRENT_ZERO_VOLTAGE`. Record normal motor current and vibration, then set the two limits with an appropriate safety margin.

## Safety

Never connect mains voltage to the ESP32 or a breadboard. Use a current sensor, fuse, enclosure, wire gauge, and isolation appropriate to the motor circuit; obtain qualified supervision for mains-powered installation.

