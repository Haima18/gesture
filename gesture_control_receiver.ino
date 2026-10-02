#include <WiFi.h>
#include <esp_now.h>

// ================= MOTOR PINS =================
// Motor A = LEFT motor, Motor B = RIGHT motor
#define ENA 25
#define IN1 26
#define IN2 27

#define ENB 13
#define IN3 14
#define IN4 12

// ================= SETTINGS =================
#define SPEED 170            // same speed for forward, backward, left, right
#define COMMAND_TIMEOUT 1000

typedef struct {
  char command;
} Message;

Message message = {'S'};

unsigned long lastCommandTime = 0;

// ================= SINGLE MOTOR CONTROL =================

void leftCW() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  ledcWrite(ENA, SPEED);
}

void leftCCW() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  ledcWrite(ENA, SPEED);
}

void rightCW() {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(ENB, SPEED);
}

void rightCCW() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  ledcWrite(ENB, SPEED);
}

// ================= ROBOT MOVEMENTS =================

void stopRobot() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);
}

void forwardRobot()  { leftCW();  rightCW();  }
void backwardRobot() { leftCCW(); rightCCW(); }
void leftRobot()     { leftCW();  rightCCW(); }   // L: left CW, right CCW
void rightRobot()    { leftCCW(); rightCW();  }   // R: left CCW, right CW

// ================= ESP-NOW RECEIVE =================

void OnDataRecv(const esp_now_recv_info_t *info,
                const uint8_t *incomingData,
                int len) {

  if (len != sizeof(Message)) return;

  memcpy(&message, incomingData, sizeof(message));
  lastCommandTime = millis();

  Serial.print("Received: ");
  Serial.println(message.command);
}

// ================= SETUP =================

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  ledcAttach(ENA, 1000, 8);
  ledcAttach(ENB, 1000, 8);

  stopRobot();

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW INIT FAILED!");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("ROBOT READY");

  lastCommandTime = millis();
}

// ================= LOOP =================

void loop() {

  if (millis() - lastCommandTime > COMMAND_TIMEOUT) {
    stopRobot();
    return;
  }

  char cmd = message.command;

  if (cmd == 'F')      forwardRobot();
  else if (cmd == 'B') backwardRobot();
  else if (cmd == 'L') leftRobot();
  else if (cmd == 'R') rightRobot();
  else                 stopRobot();

  delay(20);
}
