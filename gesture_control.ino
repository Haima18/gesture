#include <Wire.h>
#include <WiFi.h>
#include <esp_now.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// ---------- I2C PINS (change these two if needed) ----------
#define SDA_PIN 25
#define SCL_PIN 26

// ---------- Robot ESP32 MAC address ----------
uint8_t robotMAC[] = {0x98, 0xF4, 0xAB, 0x09, 0x61, 0xB4};

// ---------- Settings ----------
const float THRESHOLD = 3.0;
const int READ_DELAY = 150;

Adafruit_MPU6050 mpu;

typedef struct {
  char command;
} Message;

Message message;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!mpu.begin()) {
    Serial.println("MPU6050 NOT FOUND - check wiring");
    while (true) delay(1000);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW INIT FAILED");
    while (true) delay(1000);
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, robotMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("FAILED TO ADD ROBOT");
    while (true) delay(1000);
  }

  Serial.println("READY");
}

void loop() {
  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);

  float X = a.acceleration.x;
  float Y = a.acceleration.y;

  char command = 'S';

  if (Y > THRESHOLD)        command = 'F';
  else if (Y < -THRESHOLD)  command = 'B';
  else if (X > THRESHOLD)   command = 'R';
  else if (X < -THRESHOLD)  command = 'L';

  message.command = command;
  esp_err_t result = esp_now_send(robotMAC, (uint8_t *)&message, sizeof(message));

  Serial.print("X: ");
  Serial.print(X, 2);
  Serial.print(" | Y: ");
  Serial.print(Y, 2);
  Serial.print(" | Command: ");
  Serial.print(command);
  Serial.println(result == ESP_OK ? " | Sent" : " | Send FAILED");

  delay(READ_DELAY);
}