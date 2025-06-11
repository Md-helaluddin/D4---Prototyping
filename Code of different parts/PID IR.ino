// === Motor control pins ===
const int mr1 = 2;  // Left motor IN1 (reversed wiring)
const int mr2 = 3;  // Left motor IN2 (reversed wiring)
const int ml1 = 4;  // Right motor IN1
const int ml2 = 5;  // Right motor IN2

// === Enable (PWM) pins ===
const int enA = 6;  // Enable pin for left motor
const int enB = 9;  // Enable pin for right motor

// === IR sensor pins ===
const int leftIR  = A0;  // Left IR sensor
const int rightIR = A1;  // Right IR sensor

// === PID Control Variables ===
float Kp = 25.0;    // Proportional gain (reduced for lower speeds)
float Ki = 0.05;    // Integral gain (reduced for stability)
float Kd = 8.0;     // Derivative gain (reduced for smoother control)

float lastError = 0;
float integral = 0;
int baseSpeed = 60;     // Base speed for both motors
int maxSpeed = 80;      // Maximum motor speed
int minSpeed = 30;      // Minimum motor speed

unsigned long lastTime = 0;

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
  Serial.println("PID Line Follower Started");
  Serial.println("Kp=" + String(Kp) + " Ki=" + String(Ki) + " Kd=" + String(Kd));
}

void loop() {
  int leftSensor = digitalRead(leftIR);   // LOW = black line detected
  int rightSensor = digitalRead(rightIR);

  // Calculate error based on sensor readings
  int error = calculateError(leftSensor, rightSensor);
  
  // Calculate PID output
  float pidOutput = calculatePID(error);
  
  // Apply PID correction to motor speeds
  int leftSpeed = constrain(baseSpeed - pidOutput, minSpeed, maxSpeed);
  int rightSpeed = constrain(baseSpeed + pidOutput, minSpeed, maxSpeed);
  
  // Move motors with calculated speeds
  moveWithSpeed(leftSpeed, rightSpeed);
  
  // Debug output
  Serial.print("L:");
  Serial.print(leftSensor);
  Serial.print(" R:");
  Serial.print(rightSensor);
  Serial.print(" Err:");
  Serial.print(error);
  Serial.print(" PID:");
  Serial.print(pidOutput);
  Serial.print(" Speeds L:");
  Serial.print(leftSpeed);
  Serial.print(" R:");
  Serial.println(rightSpeed);

  delay(10); // Faster loop for smoother control
}

// Calculate error based on sensor readings
int calculateError(int left, int right) {
  if (left == 0 && right == 0) {
    // Both sensors on black line - no error
    return 0;
  }
  else if (left == 0 && right == 1) {
    // Left on black, right on white - line is to the left
    return -1;
  }
  else if (left == 1 && right == 0) {
    // Left on white, right on black - line is to the right
    return 1;
  }
  else {
    // Both sensors on white - maintain last error direction
    return (lastError > 0) ? 2 : -2;  // Stronger correction when lost
  }
}

// Calculate PID output based on error
float calculatePID(int error) {
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastTime) / 1000.0; // Convert to seconds
  
  if (deltaTime <= 0) deltaTime = 0.01; // Prevent division by zero
  
  // Proportional term
  float proportional = error;
  
  // Integral term (with windup protection)
  integral += error * deltaTime;
  integral = constrain(integral, -100, 100); // Prevent integral windup
  
  // Derivative term
  float derivative = (error - lastError) / deltaTime;
  
  // Calculate PID output
  float output = (Kp * proportional) + (Ki * integral) + (Kd * derivative);
  
  // Update variables for next iteration
  lastError = error;
  lastTime = currentTime;
  
  return output;
}

// Move robot with specified speeds for each motor
void moveWithSpeed(int leftSpeed, int rightSpeed) {
  // Left motor control
  if (leftSpeed >= 0) {
    digitalWrite(mr1, LOW);
    digitalWrite(mr2, HIGH);
    analogWrite(enA, abs(leftSpeed));
  } else {
    digitalWrite(mr1, HIGH);
    digitalWrite(mr2, LOW);
    analogWrite(enA, abs(leftSpeed));
  }
  
  // Right motor control
  if (rightSpeed >= 0) {
    digitalWrite(ml1, HIGH);
    digitalWrite(ml2, LOW);
    analogWrite(enB, abs(rightSpeed));
  } else {
    digitalWrite(ml1, LOW);
    digitalWrite(ml2, HIGH);
    analogWrite(enB, abs(rightSpeed));
  }
}

// Emergency stop function
void stopMotors() {
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, LOW);
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, LOW);
}

// Function to tune PID parameters during runtime (optional)
void tunePID(float newKp, float newKi, float newKd) {
  Kp = newKp;
  Ki = newKi;
  Kd = newKd;
  integral = 0; // Reset integral when changing parameters
  Serial.println("PID Updated: Kp=" + String(Kp) + " Ki=" + String(Ki) + " Kd=" + String(Kd));
}