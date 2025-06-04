// === Motor control pins ===
const int mr1 = 2;  // Left motor IN1 (reversed wiring)
const int mr2 = 3;  // Left motor IN2 (reversed wiring)
const int ml1 = 4;  // Right motor IN1
const int ml2 = 5;  // Right motor IN2

// === Enable (PWM) pins ===
const int enA = 6;  // Enable pin for left motor
const int enB = 9;  // Enable pin for right motor

// === IR sensor pins ===
const int leftIR  = A0;
const int rightIR = A1;

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
  int left = digitalRead(leftIR);   // 1 = black, 0 = white
  int right = digitalRead(rightIR);

  Serial.print("Left IR: "); Serial.print(left);
  Serial.print(" | Right IR: "); Serial.println(right);

  if (left == 0 && right == 0) {
    moveForward(70);  // both on white — go straight
  }
  else if (left == 1 && right == 0) {
    slightRight(60);  // left on black — back left, fwd right
  }
  else if (left == 0 && right == 1) {
    slightLeft(60);   // right on black — fwd left, back right
  }
  else {
    stopMotors();     // both on black — stop
  }

  delay(15);  // shorter delay = smoother control
}

// === Motor Functions ===

void moveForward(int speedVal) {
  analogWrite(enA, speedVal);
  analogWrite(enB, speedVal);

  digitalWrite(mr1, LOW);   // left motor forward
  digitalWrite(mr2, HIGH);  // reversed wiring

  digitalWrite(ml1, HIGH);  // right motor forward
  digitalWrite(ml2, LOW);
}

void slightLeft(int speedVal) {
  analogWrite(enA, speedVal);
  analogWrite(enB, speedVal);

  // Left motor forward (reversed)
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);

  // Right motor backward
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, HIGH);
}

void slightRight(int speedVal) {
  analogWrite(enA, speedVal);
  analogWrite(enB, speedVal);

  // Left motor backward (reversed)
  digitalWrite(mr1, HIGH);
  digitalWrite(mr2, LOW);

  // Right motor forward
  digitalWrite(ml1, HIGH);
  digitalWrite(ml2, LOW);
}

void stopMotors() {
  analogWrite(enA, 0);
  analogWrite(enB, 0);

  digitalWrite(mr1, LOW);
  digitalWrite(mr2, LOW);
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, LOW);
}
