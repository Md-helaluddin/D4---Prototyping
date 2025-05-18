#include <Servo.h>

// ——— Motor pins (from datasheet) ———
const int mr1 = 2;
const int mr2 = 3;
const int ml1 = 4;
const int ml2 = 5;

// ——— IR sensor pins (A0 and A1) ———
const int leftIRPin  = A0;
const int rightIRPin = A1;

// ——— Ultrasonic pins ———
const int trigPin = 10;
const int echoPin = 11;

// ——— Servo motor pin ———
const int servoPin = 9;
Servo scanServo;

// ——— Obstacle detection ———
#define BUFFER_SIZE 5
int distanceBuffer[BUFFER_SIZE];
int bufferIndex = 0;

int distance_F = 100;
int distance_L = 100;
int distance_R = 100;

void setup() {
  Serial.begin(9600);

  pinMode(leftIRPin, INPUT);
  pinMode(rightIRPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(mr1, OUTPUT);
  pinMode(mr2, OUTPUT);
  pinMode(ml1, OUTPUT);
  pinMode(ml2, OUTPUT);

  scanServo.attach(servoPin);
  scanServo.write(90);  // center

  Stop();
  delay(500);

  for (int i = 0; i < BUFFER_SIZE; i++) distanceBuffer[i] = 100;
}

void loop() {
  distance_F = Ultrasonic_read();
  Serial.print("Distance Front: ");
  Serial.println(distance_F);

  distanceBuffer[bufferIndex] = distance_F;
  bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;

  int lowCount = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (distanceBuffer[i] > 0 && distanceBuffer[i] <= 13) {
      lowCount++;
    }
  }

  if (lowCount >= 3) {
    Serial.println("Obstacle detected! Checking sides...");
    Stop();
    delay(200);
    Check_side();
    return;
  }

  int L = digitalRead(leftIRPin);
  int R = digitalRead(rightIRPin);

  if (L == LOW && R == LOW) {
    Forward();
  } else if (L == HIGH && R == LOW) {
    turnLeft();
  } else if (L == LOW && R == HIGH) {
    turnRight();
  } else {
    Forward();
  }

  delay(10);
}

// ——— Servo scanning and avoidance maneuver ———
void Check_side() {
  Stop();

  // Scan right
  scanServo.write(135);
  delay(400);
  distance_R = Ultrasonic_read();
  Serial.print("Right: "); Serial.println(distance_R);

  // Scan left
  scanServo.write(45);
  delay(500);
  distance_L = Ultrasonic_read();
  Serial.print("Left: "); Serial.println(distance_L);

  // Recenter
  scanServo.write(90);
  delay(300);

  if (distance_L > distance_R) {
    Serial.println("Turning LEFT to avoid obstacle");
    turnLeft();  delay(600);
    Forward();   delay(2000);
    turnRight(); delay(600);
    Forward();   delay(2000);
    turnRight(); delay(600);
  } else {
    Serial.println("Turning RIGHT to avoid obstacle");
    turnRight(); delay(600);
    Forward();   delay(2000);
    turnLeft();  delay(600);
    Forward();   delay(2000);
    turnLeft();  delay(600);
  }

  Stop();
  delay(200);

  for (int i = 0; i < BUFFER_SIZE; i++) {
    distanceBuffer[i] = 100;
  }

  Serial.println("Obstacle avoided. Resuming normal line-following.");
}

// ——— Ultrasonic reading ———
int Ultrasonic_read() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return 100;
  int cm = duration / 29 / 2;
  return (cm > 0 && cm <= 100) ? cm : 100;
}

// ——— Motor control ———
void Forward() {
  digitalWrite(mr1, HIGH); digitalWrite(mr2, LOW);
  digitalWrite(ml1, LOW);  digitalWrite(ml2, HIGH);
}

void Backward() {
  digitalWrite(mr1, LOW);  digitalWrite(mr2, HIGH);
  digitalWrite(ml1, HIGH); digitalWrite(ml2, LOW);
}

void turnLeft() {
  digitalWrite(mr1, HIGH); digitalWrite(mr2, LOW);
  digitalWrite(ml1, HIGH); digitalWrite(ml2, LOW);
}

void turnRight() {
  digitalWrite(mr1, LOW);  digitalWrite(mr2, HIGH);
  digitalWrite(ml1, LOW);  digitalWrite(ml2, HIGH);
}

void Stop() {
  digitalWrite(mr1, LOW); digitalWrite(mr2, LOW);
  digitalWrite(ml1, LOW); digitalWrite(ml2, LOW);
}
