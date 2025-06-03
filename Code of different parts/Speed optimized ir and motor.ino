// === Motor control pins ===
const int mr1 = 2;  // Left motor IN1 (reversed wiring)
const int mr2 = 3;  // Left motor IN2 (reversed wiring)
const int ml1 = 4;  // Right motor IN1
const int ml2 = 5;  // Right motor IN2

// === Enable (PWM) pins ===
const int enA = 6;  // Left motor speed
const int enB = 9;  // Right motor speed

int speed = 80;
int add = 0;
int sub = 80;

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
  int left = digitalRead(leftIR);   // LOW = black line
  int right = digitalRead(rightIR);

  Serial.print("Left IR: "); Serial.print(left);
  Serial.print(" | Right IR: "); Serial.println(right);

  if (left == 0 && right == 0) {
    moveForward(speed);
  }
  else if (left == 0 && right == 1) {
    turnLeft(speed);
  }
  else if (left == 1 && right == 0) {
    turnRight(speed);
  }
  else {
    stopMotors();
  }

  delay(20);
}

// === Motor Control ===

void moveForward(int speedVal) {
  analogWrite(enA, speedVal);
  analogWrite(enB, speedVal);

  // Left motor forward
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);

  // Right motor forward
  digitalWrite(ml1, HIGH);
  digitalWrite(ml2, LOW);
}

void turnLeft(int speedVal) {
  analogWrite(enA, speedVal + add); // Left motor faster
  analogWrite(enB, speedVal - sub); // Right motor slower

  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);

  digitalWrite(ml1, HIGH);
  digitalWrite(ml2, LOW);
}

void turnRight(int speedVal) {
  analogWrite(enA, speedVal - sub); // Left motor slower
  analogWrite(enB, speedVal + add); // Right motor faster

  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);

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
