#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <espnow.h>
#include <ESP8266WiFi.h>

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Motor control pins
#define IN1 D3
#define IN2 D4
#define ENA D5

// Button pins
#define ACCELERATE_BTN D6
#define BRAKE_BTN D7

// Speed variables
int motorPWM = 0;
int kmph = 0;
const int maxPWM = 170;
const int maxKMPH = 140;
const int speedStep = 20;

// Button state tracking
unsigned long accelPressStart = 0;
bool accelWasPressed = false;
bool accelLongPress = false;

unsigned long brakePressStart = 0;
bool brakeWasPressed = false;
bool brakeLongPress = false;

// ESPNOW peer MAC (change if needed)
uint8_t peerAddress[] = {0xEC, 0xFA, 0xBC, 0x77, 0x16, 0x57}; // Replace with the peer ESP Mac address

typedef struct struct_message {
  char action[15];
} struct_message;

struct_message incomingData;

void setup() {
  Serial.begin(9600);
  delay(1000);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ACCELERATE_BTN, INPUT_PULLUP);
  pinMode(BRAKE_BTN, INPUT_PULLUP);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  lcd.init();
  lcd.backlight();
  showStartupScreen();

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_add_peer(peerAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_register_recv_cb(onDataReceive);
}

void loop() {
  handleButtons();
}

void handleButtons() {
  unsigned long now = millis();

  // ACCELERATE
  bool accelPressed = (digitalRead(ACCELERATE_BTN) == LOW);
  if (accelPressed && !accelWasPressed) {
    accelPressStart = now;
    accelWasPressed = true;
    accelLongPress = false;
  } else if (accelPressed && accelWasPressed && !accelLongPress && (now - accelPressStart > 1000)) {
    accelLongPress = true;
    emergencyBrake();
  } else if (!accelPressed && accelWasPressed) {
    if (!accelLongPress) {
      kmph += speedStep;
      kmph = constrain(kmph, 0, maxKMPH);
      motorPWM = map(kmph, 0, maxKMPH, 0, maxPWM);
      
      // Re-assert direction
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);

      analogWrite(ENA, motorPWM);
      updateLCD();
    }
    accelWasPressed = false;
  }

  // BRAKE
  bool brakePressed = (digitalRead(BRAKE_BTN) == LOW);
  if (brakePressed && !brakeWasPressed) {
    brakePressStart = now;
    brakeWasPressed = true;
    brakeLongPress = false;
  } else if (brakePressed && brakeWasPressed && !brakeLongPress && (now - brakePressStart > 1000)) {
    brakeLongPress = true;
    kmph -= 20;
    kmph = constrain(kmph, 0, maxKMPH);
    motorPWM = map(kmph, 0, maxKMPH, 0, maxPWM);

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, motorPWM);
    updateLCD();
  } else if (!brakePressed && brakeWasPressed) {
    if (!brakeLongPress) {
      suddenBrake();
    }
    brakeWasPressed = false;
  }
}

void suddenBrake() {
  kmph = 0;
  motorPWM = 0;
  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("!! EMERGENCY !!");
  lcd.setCursor(0, 1);
  lcd.print("Sudden Brake!");

  Serial.println("SUDDEN BRAKE");
  sendESPAlert("BRAKE");

  delay(2000);
  updateLCD();
}

void emergencyBrake() {
  // kmph = 0;
  // motorPWM = 0;
  // analogWrite(ENA, 0);
  // digitalWrite(IN1, LOW);
  // digitalWrite(IN2, LOW);


  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("EMERGENCY!");
  lcd.setCursor(1, 1);
  lcd.print("||||||||||||||");

  Serial.println("EMERGENCY STOP");
  sendESPAlert("EMERGENCY");

  delay(2000);
  updateLCD();
}

void sendESPAlert(const char* type) {
  struct_message msg;
  strncpy(msg.action, type, sizeof(msg.action));
  esp_now_send(peerAddress, (uint8_t *)&msg, sizeof(msg));
}

void onDataReceive(uint8_t *mac, uint8_t *incoming, uint8_t len) {
  memcpy(&incomingData, incoming, sizeof(incomingData));
  String alert = String(incomingData.action);

  Serial.print("Received: ");
  Serial.println(alert);

  lcd.clear();
  if (alert == "EMERGENCY") {
    lcd.setCursor(0, 0);
    lcd.print("!!! EMERGENCY !!!");
    lcd.setCursor(0, 1);
    lcd.print("Stop Immediately");
  } else if (alert == "BRAKE") {
    lcd.setCursor(0, 0);
    lcd.print("Nearby Vehicle");
    lcd.setCursor(0, 1);
    lcd.print("Sudden Brake!");
  }

  // Always stop motor
  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  kmph = 0;

  delay(2000);
  updateLCD();
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Speed: ");
  lcd.print(kmph);
  lcd.print(" KMPH");

  lcd.setCursor(0, 1);
  int barLength = map(kmph, 0, maxKMPH, 0, 12);
  for (int i = 0; i < 12; i++) {
    lcd.print(i < barLength ? char(255) : '-');
  }
}

void showStartupScreen() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Smart Motor Ctrl");
  lcd.setCursor(3, 1);
  lcd.print("Initializing...");
  delay(1500);
  updateLCD();
}
