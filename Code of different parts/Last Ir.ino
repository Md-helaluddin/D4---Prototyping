// === Motor control pins ===
const int mr1 = 2;  // Left motor IN1 (reversed wiring)
const int mr2 = 3;  // Left motor IN2 (reversed wiring)
const int ml1 = 4;  // Right motor IN1
const int ml2 = 5;  // Right motor IN2

// === Enable (PWM) pins ===
const int enA = 6;  // Left motor
const int enB = 9;  // Right motor

// === IR sensors ===
const int leftIR  = A0;
const int rightIR = A1;

// === Speed Settings ===
int baseSpeed = 70;
int turnDrop  = 50;  // Reduced turning side speed

void setup() {
  pinMode(mr1, OUTPUT); pinMode(mr2, OUTPUT);
  pinMode(ml1, OUTPUT); pinMode(ml2, OUTPUT);
  pinMode(enA, OUTPUT); pinMode(enB, OUTPUT);
  pinMode(leftIR, INPUT); pinMode(rightIR, INPUT);

  Serial.begin(9600);
}

void loop() {
  int left = digitalRead(leftIR);   // 1 = black (line), 0 = white
  int right = digitalRead(rightIR);

  Serial.print("Left IR: "); Serial.print(left);
  Serial.print(" | Right IR: "); Serial.println(right);

  if (left == 0 && right == 0) {
    // Both white: go straight
    drive(baseSpeed, baseSpeed);
  }
  else if (left == 1 && right == 0) {
    // Turn left: slow down left motor
    drive(baseSpeed - turnDrop, baseSpeed);
  }
  else if (left == 0 && right == 1) {
    // Turn right: slow down right motor
    drive(baseSpeed, baseSpeed - turnDrop);
  }
  else {
    // Both black: stop
    stopMotors();
  }

  delay(10);
}

// === Core Driving Logic ===
void drive(int leftSpeed, int rightSpeed) {
  analogWrite(enA, constrain(leftSpeed, 0, 255));
  analogWrite(enB, constrain(rightSpeed, 0, 255));

  digitalWrite(mr1, LOW);  digitalWrite(mr2, HIGH); // left forward (reversed)
  digitalWrite(ml1, HIGH); digitalWrite(ml2, LOW);  // right forward
}

void stopMotors() {
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  digitalWrite(mr1, LOW); digitalWrite(mr2, LOW);
  digitalWrite(ml1, LOW); digitalWrite(ml2, LOW);
}
