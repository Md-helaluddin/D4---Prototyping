#include <Servo.h>

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

// === Servo ===
Servo steeringServo;
const int servoPin = 10;

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

  steeringServo.attach(servoPin); // Attach the servo

  Serial.begin(9600);
}

void loop() {
  int left = digitalRead(leftIR);   // LOW = black line detected
  int right = digitalRead(rightIR);

  long distance = getDistance();

  Serial.print("Left IR: "); Serial.print(left);
  Serial.print(" | Right IR: "); Serial.print(right);
  Serial.print(" | Distance: "); Serial.println(distance);

  if (distance < 40) {
    stopMotors();
    return;
  }

  if (left == 0 && right == 0) {
    moveForward(60);
    steeringServo.write(90); // straight
  }
  else if (left == 0 && right == 1) {
    turnLeft(100);
    steeringServo.write(60); // turn left
  }
  else if (left == 1 && right == 0) {
    turnRight(100);
    steeringServo.write(120); // turn right
  }
  else {
    stopMotors();
    steeringServo.write(90); // center
  }

  delay(20);
}

// === Motor Control Functions ===

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

// === Ultrasonic Function ===

long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2; // in cm
  return distance;
}
