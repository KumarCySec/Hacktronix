// === Motor A Connections (Motor 1) ===
const int in1 = D2;    // Motor A IN1
const int in2 = D3;    // Motor A IN2
const int enA = D4;    // Motor A ENA (PWM)

// === Motor B Connections (Motor 2) ===
const int in3 = D5;    // Motor B IN3
const int in4 = D6;    // Motor B IN4
const int enB = D7;    // Motor B ENB (PWM)

// === Variables ===
String command = "";
int motorSpeed = 800; // Default speed (range 0 - 1023)

void setup() {
  // Start Serial Communication
  Serial.begin(115200);

  // Wait until Serial Monitor is connected
  while (!Serial) {
    delay(10);
  }
  
  delay(2000); // Give time to reboot fully

  Serial.println();
  Serial.println("=== ESP8266 Motor Controller Ready ===");
  Serial.println("Commands:");
  Serial.println(" - forward");
  Serial.println(" - backward");
  Serial.println(" - stop");
  Serial.println(" - speed <0-1023>");

  // Set motor pins as output
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enA, OUTPUT);

  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enB, OUTPUT);

  // Set initial motor speed
  setMotorSpeed(motorSpeed);
}

void loop() {
  if (Serial.available()) {
    command = Serial.readStringUntil('\n');
    command.trim(); // Clean any newline, spaces

    if (command == "forward") {
      moveForward();
    } 
    else if (command == "backward") {
      moveBackward();
    } 
    else if (command == "stop") {
      stopMotors();
    } 
    else if (command.startsWith("speed ")) {
      int newSpeed = command.substring(6).toInt();
      if (newSpeed >= 0 && newSpeed <= 1023) {
        motorSpeed = newSpeed;
        setMotorSpeed(motorSpeed);
        Serial.print("Speed set to: ");
        Serial.println(motorSpeed);
      } else {
        Serial.println("Invalid speed! (Range: 0 - 1023)");
      }
    }
    else {
      Serial.println("Unknown command! Use: forward, backward, stop, speed <value>");
    }
  }
}

// === Motor Control Functions ===

void moveForward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  Serial.println("Moving Forward 🚗💨");
}

void moveBackward() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  Serial.println("Moving Backward 🚗🔙");
}

void stopMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  Serial.println("Motors Stopped 🛑");
}

void setMotorSpeed(int speed) {
  analogWrite(enA, speed);
  analogWrite(enB, speed);
}
