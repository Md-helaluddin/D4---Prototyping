// ================= PIN DEFINITIONS =================

// Motor pins
const int mr1 = 2;
const int mr2 = 3;
const int ml1 = 4;
const int ml2 = 5;
const int enA = 6;
const int enB = 9;

// IR sensors
const int leftIR  = A0;
const int rightIR = A1;

// Ultrasonic sensor
const int trigPin = 8;
const int echoPin = 7;

// Color sensor (TCS3200)
#define S2 A4
#define S3 A5
#define sensorOut 11

// ================= GLOBAL VARIABLES =================

int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

bool avoidingObstacle = false;

// ================= SETUP =================

void setup() {
  pinMode(mr1, OUTPUT); pinMode(mr2, OUTPUT);
  pinMode(ml1, OUTPUT); pinMode(ml2, OUTPUT);
  pinMode(enA, OUTPUT); pinMode(enB, OUTPUT);

  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  Serial.begin(9600);
  Serial.println("Robot started");
}

// ================= MAIN LOOP =================

void loop() {

  long distance = readUltrasonic();

  // ---------- OBSTACLE DETECTED ----------
  if (distance > 0 && distance <= 6 && !avoidingObstacle) {
    avoidingObstacle = true;
    stopMotors();
    delay(300);

    String color = performColorDetection();

    if (color == "BLUE") {
      pushThroughObstacle();
      avoidingObstacle = false;
    } else {
      handleObstacle();  
    }
    return;
  }

  // ---------- LINE FOLLOWING ----------
  int l = digitalRead(leftIR);
  int r = digitalRead(rightIR);

  if (l == 0 && r == 0) {
    moveForward(70, 70);
  }
  else if (l == 0 && r == 1) {
    moveWithTurn(80, -80);
  }
  else if (l == 1 && r == 0) {
    moveWithTurn(-80, 80);
  }
  else {
    stopMotors();
  }
}

// ================= MOTOR FUNCTIONS =================

void moveForward(int spA, int spB) {
  analogWrite(enA, spA);
  analogWrite(enB, spB);
  digitalWrite(mr1, LOW);  digitalWrite(mr2, HIGH);
  digitalWrite(ml1, HIGH); digitalWrite(ml2, LOW);
}

void moveForward1(int sp) {
  moveForward(sp, sp);
}

void moveBackward(int spA, int spB) {
  analogWrite(enA, spA);
  analogWrite(enB, spB);
  digitalWrite(mr1, HIGH); digitalWrite(mr2, LOW);
  digitalWrite(ml1, LOW);  digitalWrite(ml2, HIGH);
}

void moveWithTurn(int leftSpeed, int rightSpeed) {
  analogWrite(enA, abs(leftSpeed));
  analogWrite(enB, abs(rightSpeed));

  digitalWrite(mr1, leftSpeed > 0 ? LOW : HIGH);
  digitalWrite(mr2, leftSpeed > 0 ? HIGH : LOW);

  digitalWrite(ml1, rightSpeed > 0 ? HIGH : LOW);
  digitalWrite(ml2, rightSpeed > 0 ? LOW : HIGH);
}

void stopMotors() {
  analogWrite(enA, 0);
  analogWrite(enB, 0);
}

// ================= ULTRASONIC =================

long readUltrasonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000);
  if (duration == 0) return -1;

  long distance = duration * 0.034 / 2;
  if (distance <= 0 || distance > 300) return -1;

  return distance;
}

// ================= COLOR DETECTION =================

String performColorDetection() {
  String c = readSingleColor();
  Serial.print("Detected Color: ");
  Serial.println(c);
  return c;
}

String readSingleColor() {

  digitalWrite(S2, LOW); digitalWrite(S3, LOW);
  delay(20);
  redFrequency = pulseIn(sensorOut, LOW);

  digitalWrite(S2, HIGH); digitalWrite(S3, HIGH);
  delay(20);
  greenFrequency = pulseIn(sensorOut, LOW);

  digitalWrite(S2, LOW); digitalWrite(S3, HIGH);
  delay(20);
  blueFrequency = pulseIn(sensorOut, LOW);

  Serial.print("R: "); Serial.print(redFrequency);
  Serial.print(" G: "); Serial.print(greenFrequency);
  Serial.print(" B: "); Serial.println(blueFrequency);

  return detectColor();
}

String detectColor() {
  if (redFrequency < greenFrequency && redFrequency < blueFrequency) return "RED";
  if (greenFrequency < redFrequency && greenFrequency < blueFrequency) return "GREEN";
  if (blueFrequency < redFrequency && blueFrequency < greenFrequency) return "BLUE";
  return "UNKNOWN";
}

// ================= Finding Line =================

void waitForLineDetection() {
  Serial.println("Searching for line...");
  while (true) {
    int l = digitalRead(leftIR);
    int r = digitalRead(rightIR);

    if (l == 1 || r == 1) {
      Serial.println("Line detected!");
      break;
    } else {
      moveForward1(50);
    }
    delay(10);
  }
}

// ================= Avoiding Obstacle =================

void handleObstacle() {

  stopMotors();
  delay(300);

  moveBackward(80, 80);
  delay(500);

  stopMotors();
  delay(300);

  moveWithTurn(-180, 180);
  delay(180);

  stopMotors();
  delay(300);

  moveForward(65, 65);
  delay(1600);

  stopMotors();
  delay(300);

  moveWithTurn(200, -200);
  delay(280);

  stopMotors();
  delay(300);

  moveForward1(80);
  delay(300);

  waitForLineDetection();
  avoidingObstacle = false;
}

// ================= BLUE OBSTACLE =================

void pushThroughObstacle() {
  moveForward(100, 100);
  delay(600);
  stopMotors();
}
