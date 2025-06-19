// TCS3200/TCS230 Color Sensor Pin Definitions
#define S0 A2
#define S1 A3
#define S2 A4
#define S3 A5
#define sensorOut 11
#define OE 13

// Variables for color detection
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

// Calibration values (adjust these based on your sensor and lighting conditions)
int redMin = 50, redMax = 200;
int greenMin = 60, greenMax = 220;
int blueMin = 40, blueMax = 180;

void setup() {
  // Set pins as outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OE, OUTPUT);
  
  // Set sensorOut as input
  pinMode(sensorOut, INPUT);
  
  // Initialize Output Enable (active low)
  digitalWrite(OE, LOW);
  
  // Set frequency scaling to 20% (S0=HIGH, S1=LOW)
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Color Detection System Started");
  Serial.println("Place object in front of sensor...");
}

void loop() {
  // Read Red frequency
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  redFrequency = pulseIn(sensorOut, LOW);
  
  delay(100);
  
  // Read Green frequency
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  greenFrequency = pulseIn(sensorOut, LOW);
  
  delay(100);
  
  // Read Blue frequency
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blueFrequency = pulseIn(sensorOut, LOW);
  
  delay(100);
  
  // Print raw frequency values for debugging
  Serial.print("Red: ");
  Serial.print(redFrequency);
  Serial.print(" | Green: ");
  Serial.print(greenFrequency);
  Serial.print(" | Blue: ");
  Serial.print(blueFrequency);
  Serial.print(" | Total: ");
  Serial.print(redFrequency + greenFrequency + blueFrequency);
  
  // Detect and display color
  String detectedColor = detectColor();
  Serial.print(" | Detected: ");
  Serial.println(detectedColor);
  
  delay(1000);
}

String detectColor() {
  // Check if object is actually present (minimum signal strength)
  int totalSignal = redFrequency + greenFrequency + blueFrequency;
  
  // If total signal is too high, no object is present (only ambient light)
  if (totalSignal > 3000) {  // Adjust this threshold based on your environment
    return "NO OBJECT";
  }
  
  // If total signal is too low, sensor might be covered or malfunctioning
  if (totalSignal < 100) {
    return "SENSOR ERROR";
  }
  
  // Calculate the difference between colors for better detection
  int redGreenDiff = abs(redFrequency - greenFrequency);
  int redBlueDiff = abs(redFrequency - blueFrequency);
  int greenBlueDiff = abs(greenFrequency - blueFrequency);
  
  // Minimum difference threshold to avoid false positives
  int minDifference = 20;  // Adjust this value as needed
  
  // Advanced color detection with improved logic
  if (redFrequency < greenFrequency && redFrequency < blueFrequency) {
    // Red might be dominant, but check if difference is significant
    if (redGreenDiff > minDifference && redBlueDiff > minDifference) {
      // Additional check: red should be significantly lower than others
      if (redFrequency < (greenFrequency * 0.8) && redFrequency < (blueFrequency * 0.8)) {
        return "RED";
      }
    }
  }
  else if (greenFrequency < redFrequency && greenFrequency < blueFrequency) {
    if (redGreenDiff > minDifference && greenBlueDiff > minDifference) {
      if (greenFrequency < (redFrequency * 0.8) && greenFrequency < (blueFrequency * 0.8)) {
        return "GREEN";
      }
    }
  }
  else if (blueFrequency < redFrequency && blueFrequency < greenFrequency) {
    if (redBlueDiff > minDifference && greenBlueDiff > minDifference) {
      if (blueFrequency < (redFrequency * 0.8) && blueFrequency < (greenFrequency * 0.8)) {
        return "BLUE";
      }
    }
  }
  
  // Check if all colors are similar (white/gray object or ambient light)
  if (redGreenDiff < minDifference && redBlueDiff < minDifference && greenBlueDiff < minDifference) {
    return "WHITE/GRAY";
  }
  
  return "UNKNOWN";
}

// Function to calibrate the sensor (call this to get min/max values)
void calibrateSensor() {
  Serial.println("Calibration Mode - Place RED object and observe values");
  delay(5000);
  
  for (int i = 0; i < 10; i++) {
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    int red = pulseIn(sensorOut, LOW);
    Serial.print("Red calibration: ");
    Serial.println(red);
    delay(500);
  }
  
  Serial.println("Now place GREEN object");
  delay(5000);
  
  for (int i = 0; i < 10; i++) {
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    int green = pulseIn(sensorOut, LOW);
    Serial.print("Green calibration: ");
    Serial.println(green);
    delay(500);
  }
  
  Serial.println("Now place BLUE object");
  delay(5000);
  
  for (int i = 0; i < 10; i++) {
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    int blue = pulseIn(sensorOut, LOW);
    Serial.print("Blue calibration: ");
    Serial.println(blue);
    delay(500);
  }
}