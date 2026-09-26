/*
  IoT Motor Health Guard — ESP32 starter
  Sensors: MPU6050 (vibration/tilt) + ACS712 (motor current)
  Sends telemetry to an MQTT broker and alarms with an LED/buzzer.
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// --- Change these before uploading ---
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* MQTT_HOST = "broker.hivemq.com";  // Replace for a private deployment.
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "iot-motor-health-guard/telemetry";

// Wiring
constexpr int CURRENT_PIN = 34;   // ACS712 OUT → ESP32 ADC pin
constexpr int LED_PIN = 2;
constexpr int BUZZER_PIN = 25;

// ACS712 calibration. Measure the ADC value while no current is flowing and
// replace this value. For a 5 A ACS712 board, sensitivity is 0.185 V/A.
constexpr float ADC_REFERENCE_VOLTAGE = 3.3F;
constexpr float CURRENT_ZERO_VOLTAGE = 1.65F;
constexpr float ACS712_VOLTS_PER_AMP = 0.185F;

// Health limits — tune these to the motor's normal operating profile.
constexpr float MAX_RMS_CURRENT_A = 4.0F;
constexpr float MAX_VIBRATION_MS2 = 18.0F;
constexpr unsigned long PUBLISH_INTERVAL_MS = 5000;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
Adafruit_MPU6050 mpu;
unsigned long lastPublish = 0;

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void connectMqtt() {
  while (!mqtt.connected()) {
    String clientId = "motor-guard-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    if (!mqtt.connect(clientId.c_str())) delay(2000);
  }
}

float readRmsCurrent() {
  constexpr int samples = 500;
  float sumSquares = 0;
  for (int i = 0; i < samples; i++) {
    float voltage = analogRead(CURRENT_PIN) * ADC_REFERENCE_VOLTAGE / 4095.0F;
    float amps = (voltage - CURRENT_ZERO_VOLTAGE) / ACS712_VOLTS_PER_AMP;
    sumSquares += amps * amps;
    delayMicroseconds(200);
  }
  return sqrt(sumSquares / samples);
}

float readVibrationMagnitude() {
  sensors_event_t accel, gyro, temperature;
  mpu.getEvent(&accel, &gyro, &temperature);
  return sqrt(accel.acceleration.x * accel.acceleration.x +
              accel.acceleration.y * accel.acceleration.y +
              accel.acceleration.z * accel.acceleration.z);
}

void setAlarm(bool alarm) {
  digitalWrite(LED_PIN, alarm ? HIGH : LOW);
  digitalWrite(BUZZER_PIN, alarm ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  analogReadResolution(12);

  Wire.begin();
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found. Check SDA/SCL wiring.");
    while (true) delay(1000);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);

  connectWiFi();
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  if (!mqtt.connected()) connectMqtt();
  mqtt.loop();

  if (millis() - lastPublish < PUBLISH_INTERVAL_MS) return;
  lastPublish = millis();

  const float currentA = readRmsCurrent();
  const float vibration = readVibrationMagnitude();
  const bool overload = currentA > MAX_RMS_CURRENT_A;
  const bool excessiveVibration = vibration > MAX_VIBRATION_MS2;
  const bool alarm = overload || excessiveVibration;
  setAlarm(alarm);

  char payload[240];
  snprintf(payload, sizeof(payload),
    "{\"current_a\":%.2f,\"vibration_ms2\":%.2f,\"overload\":%s,\"excessive_vibration\":%s,\"status\":\"%s\"}",
    currentA, vibration, overload ? "true" : "false",
    excessiveVibration ? "true" : "false", alarm ? "ALERT" : "HEALTHY");

  mqtt.publish(MQTT_TOPIC, payload);
  Serial.println(payload);
}

