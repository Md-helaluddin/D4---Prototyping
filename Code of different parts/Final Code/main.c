// === Motor control pins ===
const int mr1 = 2;  // Left motor IN1 (reversed wiring)
const int mr2 = 3;  // Left motor IN2 (reversed wiring)
const int ml1 = 4;  // Right motor IN1
const int ml2 = 5;  // Right motor IN2

// === Enable (PWM) pins ===
const int enA = 6;
const int enB = 9;

// === Ultrasonic sensor pins ===
const int trigPin = 8;
const int echoPin = 7;

// === IR sensor pins ===
const int leftIR  = A0;
const int rightIR = A1;

// === Color sensor pins ===
#define S2 A4
#define S3 A5
#define sensorOut 11

// === Color detection variables ===
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

// === Speed control ===
int speedA = 70;
int speedB = 70;

// === Ultrasonic variables ===
float distance = 10;

void setup() {
  // Motor pins
  pinMode(mr1, OUTPUT); pinMode(mr2, OUTPUT);
  pinMode(ml1, OUTPUT); pinMode(ml2, OUTPUT);
  pinMode(enA, OUTPUT); pinMode(enB, OUTPUT);

  // IR sensors
  pinMode(leftIR, INPUT); pinMode(rightIR, INPUT);

  // Ultrasonic sensor
  pinMode(trigPin, OUTPUT); pinMode(echoPin, INPUT);

  // Color sensor
  pinMode(S2, OUTPUT); pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  Serial.begin(9600);
  Serial.println("Robot with Color Detection Started");
}

void loop() {
  distance = readUltrasonic();

  // Obstacle detection
  if (distance > 0 && distance <= 6) {
    stopMotors();
    delay(500);

    String detectedColor = performColorDetection();
    Serial.print("Final Color Decision: ");
    Serial.println(detectedColor);

    if (detectedColor == "BLUE") {
      Serial.println("BLUE detected - Pushing through obstacle");
      pushThroughObstacle();
    } else {
      Serial.println("Other color - Performing manual overtaking");
      handleObstacle();
    }
    return;  // Skip line following while handling obstacle
  }

  // Normal line following
  int left = digitalRead(leftIR);
  int right = digitalRead(rightIR);

  if (left == 0 && right == 0) {
    moveForward(speedA, speedB);
  }
  else if (left == 0 && right == 1) {
    leftForward(80);
    rightBackward(60);
  }
  else if (left == 1 && right == 0) {
    leftBackward(60);
    rightForward(80);
  }
  else {
    stopMotors();
  }
}

// ================== MOTOR FUNCTIONS ==================
void moveForward(int speedVal1, int speedVal2) {
  analogWrite(enA, speedVal1);
  analogWrite(enB, speedVal2);
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);
  digitalWrite(ml1, HIGH);
  digitalWrite(ml2, LOW);
}

void moveBackward(int speedVal1, int speedVal2) {
  analogWrite(enA, speedVal1);
  analogWrite(enB, speedVal2);
  digitalWrite(mr1, HIGH);
  digitalWrite(mr2, LOW);
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, HIGH);
}

void leftForward(int speedVal) {
  analogWrite(enA, speedVal);
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);
}

void leftBackward(int speedVal) {
  analogWrite(enA, speedVal);
  digitalWrite(mr1, HIGH);
  digitalWrite(mr2, LOW);
}

void rightForward(int speedVal) {
  analogWrite(enB, speedVal);
  digitalWrite(ml1, HIGH);
  digitalWrite(ml2, LOW);
}

void rightBackward(int speedVal) {
  analogWrite(enB, speedVal);
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, HIGH);
}

void stopMotors() {
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  digitalWrite(mr1, LOW); digitalWrite(mr2, LOW);
  digitalWrite(ml1, LOW); digitalWrite(ml2, LOW);
}

// ================== ULTRASONIC FUNCTION ==================
long readUltrasonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000);
  if (duration == 0) return -1;

  long distance = duration * 0.034 / 2;
  if (distance > 300 || distance <= 0) return -1;

  return distance;
}

// ================== COLOR DETECTION FUNCTIONS ==================
String performColorDetection() {
  String color = readSingleColor();
  Serial.print("Detected Color: ");
  Serial.println(color);
  return color;
}

String readSingleColor() {
  // RED
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  delay(20);
  redFrequency = pulseIn(sensorOut, LOW);

  // GREEN
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  delay(20);
  greenFrequency = pulseIn(sensorOut, LOW);

  // BLUE
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  delay(20);
  blueFrequency = pulseIn(sensorOut, LOW);

  Serial.print("R: "); Serial.print(redFrequency);
  Serial.print(" G: "); Serial.print(greenFrequency);
  Serial.print(" B: "); Serial.println(blueFrequency);

  return detectColor();
}

String detectColor() {
  if (redFrequency == 0 || greenFrequency == 0 || blueFrequency == 0) {
    return "UNKNOWN";
  }

  if (redFrequency < greenFrequency && redFrequency < blueFrequency) return "RED";
  if (greenFrequency < redFrequency && greenFrequency < blueFrequency) return "GREEN";
  if (blueFrequency < redFrequency && blueFrequency < greenFrequency) return "BLUE";

  return "UNKNOWN";
}

// ================== OBSTACLE HANDLING ==================
void pushThroughObstacle() {
  moveForward(100, 100);
  delay(600);
  stopMotors();
  delay(200);
  moveBackward(100, 100);
  delay(600);
  stopMotors();
  delay(200);
}

void handleObstacle() {
  stopMotors();
  delay(300);
  moveBackward(80, 80); delay(500); stopMotors(); delay(300);
  moveForward(65, 65); delay(1600); stopMotors(); delay(300);
}
