// Motor pins
const int mr1 = 7;
const int mr2 = 6;
const int ml1 = 9;
const int ml2 = 8;

// IR sensor pins
const int leftIRPin = 4;
const int rightIRPin = 5;

// Simulated ultrasonic sensor pins
const int trigPin = 10;
const int echoPin = 11;

// Distance simulation
int distance = 100;

// Timing variables
unsigned long previousMillis = 0;
const unsigned long sensorInterval = 100;

enum RobotState {
  FOLLOW_LINE,
  OBSTACLE_DETECTED,
  MOVE_BACK,
  AVOID_LEFT,
  REJOIN_LINE
};

RobotState state = FOLLOW_LINE;
unsigned long stateStartTime = 0;

void setup() {
  pinMode(mr1, OUTPUT);
  pinMode(mr2, OUTPUT);
  pinMode(ml1, OUTPUT);
  pinMode(ml2, OUTPUT);

  pinMode(leftIRPin, INPUT);
  pinMode(rightIRPin, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  stopMotors();
  Serial.println("System Initialized...");
}

void loop() {
  unsigned long currentMillis = millis();

  // Simulate distance decreasing
  if (currentMillis - previousMillis >= sensorInterval) {
    previousMillis = currentMillis;
    if (distance > 10) {
      distance -= 1;
    }

    Serial.print("Simulated Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  // Always print IR sensor values for debugging
  int leftSensor = digitalRead(leftIRPin);
  int rightSensor = digitalRead(rightIRPin);

  Serial.print("Left IR: ");
  Serial.print(leftSensor);
  Serial.print(" | Right IR: ");
  Serial.println(rightSensor);

  // MAIN STATE MACHINE
  switch (state) {
    case FOLLOW_LINE:
      if (distance <= 10) {
        stopMotors();
        state = OBSTACLE_DETECTED;
        stateStartTime = currentMillis;
      } else {
        followLine(leftSensor, rightSensor);
      }
      break;

    case OBSTACLE_DETECTED:
      if (currentMillis - stateStartTime >= 500) {
        moveBackwards();
        state = MOVE_BACK;
        stateStartTime = currentMillis;
      }
      break;

    case MOVE_BACK:
      if (currentMillis - stateStartTime >= 1000) {
        avoidObstacleLeft();
        state = AVOID_LEFT;
        stateStartTime = currentMillis;
      }
      break;

    case AVOID_LEFT:
      if (currentMillis - stateStartTime >= 1000) {
        state = REJOIN_LINE;
      }
      break;

    case REJOIN_LINE:
      if (leftSensor == HIGH || rightSensor == HIGH) {
        state = FOLLOW_LINE;
      } else {
        avoidObstacleLeft();  // Keep turning until on line
      }
      break;
  }
}

void moveForward() {
  digitalWrite(mr1, HIGH);
  digitalWrite(mr2, LOW);
  digitalWrite(ml1, HIGH);
  digitalWrite(ml2, LOW);
}

void stopMotors() {
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, LOW);
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, LOW);
}

void moveBackwards() {
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, HIGH);
}

void avoidObstacleLeft() {
  // Turn left in place
  digitalWrite(mr1, HIGH);
  digitalWrite(mr2, LOW);
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, LOW);
}

void followLine(int leftSensor, int rightSensor) {
  if (leftSensor == HIGH && rightSensor == HIGH) {
    moveForward();
  } else if (leftSensor == LOW && rightSensor == HIGH) {
    // Turn left
    digitalWrite(mr1, HIGH);
    digitalWrite(mr2, LOW);
    digitalWrite(ml1, LOW);
    digitalWrite(ml2, LOW);
  } else if (leftSensor == HIGH && rightSensor == LOW) {
    // Turn right
    digitalWrite(mr1, LOW);
    digitalWrite(mr2, LOW);
    digitalWrite(ml1, HIGH);
    digitalWrite(ml2, LOW);
  } else {
    // Off the line completely
    stopMotors();
  }
}
