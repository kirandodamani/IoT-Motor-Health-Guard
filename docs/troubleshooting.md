# Troubleshooting

## MPU6050 not found

- Confirm the sensor has 3.3 V and a shared ground with the ESP32.
- Check SDA on GPIO 21 and SCL on GPIO 22, or change `Wire.begin()` for your board.
- Run an I2C scanner to verify the sensor address.

## Current readings are wrong

- Calibrate `CURRENT_ZERO_VOLTAGE` while no current flows through the sensor.
- Confirm the ACS712 model and its volts-per-amp sensitivity match the firmware setting.
- Never let the ACS712 output exceed the ESP32's 3.3 V ADC input range.

## Dashboard remains on WAITING

- Confirm the ESP32 Serial Monitor shows published JSON.
- Use the same topic in firmware, simulator, and dashboard.
- The dashboard needs an MQTT broker with WebSocket support; ordinary port 1883 alone will not work in a browser.

## Frequent false alarms

- Mount the MPU6050 firmly to the motor housing.
- Capture normal operation values before increasing thresholds.
- Check for loose mounts, worn bearings, and electrical load spikes rather than simply disabling alerts.

