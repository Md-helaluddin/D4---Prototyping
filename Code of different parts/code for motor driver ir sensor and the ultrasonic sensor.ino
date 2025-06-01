// === Motor control pins ===
const int mr1 = 2;
const int mr2 = 3;
const int ml1 = 4;
const int ml2 = 5;

// === Enable (PWM) pins ===
const int enA = 6;
const int enB = 9;

// === IR sensor pins ===
const int leftIR  = A0;
const int rightIR = A1;

// === Ultrasonic pins ===
const int trigPin = 8;
const int echoPin = 7;

void setup() {
  pinMode(mr1, OUTPUT);
  pinMode(mr2, OUTPUT);
  pinMode(ml1, OUTPUT);
  pinMode(ml2, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);

  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
}

void loop() {
  float distance = getDistanceCM();  // Get ultrasonic distance

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // === Obstacle detected ===
  if (distance > 0 && distance <= 10.0) {
    stopMotors();
    Serial.println("Obstacle too close! Stopping.");
    delay(100);  // Short pause
    return;
  }

  // === IR Line Following Logic ===
  int left = digitalRead(leftIR);   // LOW = black line
  int right = digitalRead(rightIR);

  Serial.print("Left IR: ");
  Serial.print(left);
  Serial.print(" | Right IR: ");
  Serial.println(right);

  if (left == 0 && right == 0) {
    moveForward(60);
  }
  else if (left == 0 && right == 1) {
    turnLeft(100);
  }
  else if (left == 1 && right == 0) {
    turnRight(100);
  }
  else {
    stopMotors();
  }

  delay(20);
}

// ========== Motor control functions ==========
void moveForward(int speedVal) {
  analogWrite(enA, speedVal);
  analogWrite(enB, speedVal);
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);
  digitalWrite(ml1, HIGH);
  digitalWrite(ml2, LOW);
}

void turnLeft(int speedVal) {
  analogWrite(enA, speedVal);
  analogWrite(enB, 0);
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, LOW);
}

void turnRight(int speedVal) {
  analogWrite(enA, 0);
  analogWrite(enB, speedVal);
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, LOW);
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

// ========== Ultrasonic Distance Function ==========
float getDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000);  // Timeout at 20ms

  if (duration == 0) return -1;  // No echo detected
  return duration * 0.0343 / 2.0;
}
