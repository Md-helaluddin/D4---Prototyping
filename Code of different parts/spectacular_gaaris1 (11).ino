/* ============================================================
   SIMPLE-PID LINE FOLLOWER  (two reflectance sensors)
   - Left motor wiring is reversed (handled in drive())
   - Follows a black line on white background
   - Goes forward when both sensors read white
   ============================================================ */

const uint8_t mr1 = 2;   // Left motor IN1 (reversed)
const uint8_t mr2 = 3;   // Left motor IN2 (reversed)
const uint8_t ml1 = 4;   // Right motor IN1
const uint8_t ml2 = 5;   // Right motor IN2
const uint8_t enA = 6;   // Left motor PWM
const uint8_t enB = 9;   // Right motor PWM

const uint8_t leftIR  = A0;
const uint8_t rightIR = A1;

/* PID parameters (not used heavily here, placeholder for future tuning) */
float Kp = 20;
float Ki = 0;
float Kd = 8;

const int baseSpeed = 70;  // Normal forward speed (slow and stable)

void setup() {
  pinMode(mr1, OUTPUT); pinMode(mr2, OUTPUT);
  pinMode(ml1, OUTPUT); pinMode(ml2, OUTPUT);
  pinMode(enA, OUTPUT); pinMode(enB, OUTPUT);

  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);

  Serial.begin(115200);
  Serial.println(F("Simple Line Follower Ready"));
}

void loop() {
  int left = digitalRead(leftIR);   // LOW = white, HIGH = black
  int right = digitalRead(rightIR);

  Serial.print("Left IR: ");
  Serial.print(left);
  Serial.print(" | Right IR: ");
  Serial.println(right);

  if (left == LOW && right == LOW) {
    // Both sensors on white — go forward
    moveForward(baseSpeed);
  }
  else if (left == HIGH && right == LOW) {
    // Left sees black — turn left
    turnLeft(50);
  }
  else if (left == LOW && right == HIGH) {
    // Right sees black — turn right
    turnRight(50);
  }
  else {
    // Both see black — stop (maybe a junction or dead end)
    stopMotors();
  }

  delay(20);  // short delay for stability
}

/* ================= Motor Control ================= */

void drive(int16_t leftPWM, int16_t rightPWM) {
  leftPWM = constrain(leftPWM, -255, 255);
  rightPWM = constrain(rightPWM, -255, 255);

  // Left motor (reversed wiring)
  if (leftPWM >= 0) {
    digitalWrite(mr1, LOW);
    digitalWrite(mr2, HIGH);
    analogWrite(enA, leftPWM);
  } else {
    digitalWrite(mr1, HIGH);
    digitalWrite(mr2, LOW);
    analogWrite(enA, -leftPWM);
  }

  // Right motor
  if (rightPWM >= 0) {
    digitalWrite(ml1, HIGH);
    digitalWrite(ml2, LOW);
    analogWrite(enB, rightPWM);
  } else {
    digitalWrite(ml1, LOW);
    digitalWrite(ml2, HIGH);
    analogWrite(enB, -rightPWM);
  }
}

void moveForward(int speed) {
  drive(speed, speed);
}

void turnLeft(int speed) {
  drive(-speed, speed);  // corrected: left backward, right forward
}

void turnRight(int speed) {
  drive(speed, -speed);  // corrected: left forward, right backward
}

void stopMotors() {
  drive(0, 0);
}
