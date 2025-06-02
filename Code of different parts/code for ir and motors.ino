// === Motor control pins ===
const int mr1 = 2;  // Left motor IN1 (reversed wiring)
const int mr2 = 3;  // Left motor IN2 (reversed wiring)
const int ml1 = 4;  // Right motor IN1
const int ml2 = 5;  // Right motor IN2

// === Enable (PWM) pins ===
const int enA = 6;  // Enable pin for left motor
const int enB = 9;  // Enable pin for right motor

// === IR sensor pins ===
const int leftIR  = A0;  // Left IR sensor
const int rightIR = A1;  // Right IR sensor

void setup() {
  pinMode(mr1, OUTPUT);
  pinMode(mr2, OUTPUT);
  pinMode(ml1, OUTPUT);
  pinMode(ml2, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);

  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);

  Serial.begin(9600);
}

void loop() {
  int left = digitalRead(leftIR);   // LOW = black line detected
  int right = digitalRead(rightIR);

  Serial.print("Left IR: ");
  Serial.print(left);
  Serial.print(" | Right IR: ");
  Serial.println(right);

  if (left == 0 && right == 0) {
    // Both sensors on black (line) - go straight
    moveForward(70);
  }
  else if (left == 0 && right == 1) {
    // Left on black, right on white - veer left
    turnLeft(50);
  }
  else if (left == 1 && right == 0) {
    // Right on black, left on white - veer right
    turnRight(50);
  }
  else {
    // Both sensors on white - maybe lost line, stop briefly
    stopMotors();
  }

  delay(20); // fast reaction loop
}

// ========== Motor control functions ==========

void moveForward(int speedVal) {
  analogWrite(enA, speedVal);
  analogWrite(enB, speedVal);

  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);  // Left motor forward

  digitalWrite(ml1, HIGH);
  digitalWrite(ml2, LOW);   // Right motor forward
}

void turnLeft(int speedVal) {
  analogWrite(enA, speedVal);
  analogWrite(enB, 0);      // Stop right motor

  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);  // Left motor forward

  digitalWrite(ml1, LOW);
  digitalWrite(ml2, LOW);   // Right motor off
}

void turnRight(int speedVal) {
  analogWrite(enA, 0);      // Stop left motor
  analogWrite(enB, speedVal);

  digitalWrite(mr1, LOW);
  digitalWrite(mr2, LOW);   // Left motor off

  digitalWrite(ml1, HIGH);
  digitalWrite(ml2, LOW);   // Right motor forward
}

void stopMotors() {
  analogWrite(enA, 0);
  analogWrite(enB, 0);

  digitalWrite(mr1, LOW);
  digitalWrite(mr2, LOW);
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, LOW);
}