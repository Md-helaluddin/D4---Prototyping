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
  
  // Different thresholds for different colors
  int minDifference = 15;  // General minimum difference
  int greenMinDiff = 10;   // Lower threshold for green detection
  
  // Check if all colors are very similar first (white/gray object)
  if (redGreenDiff < 8 && redBlueDiff < 8 && greenBlueDiff < 8) {
    return "WHITE/GRAY";
  }
  
  // Find the minimum frequency (strongest color signal)
  int minFreq = min(redFrequency, min(greenFrequency, blueFrequency));
  
  // Enhanced GREEN detection logic
  if (greenFrequency == minFreq) {
    // Green is the strongest signal
    if ((redGreenDiff > greenMinDiff || greenBlueDiff > greenMinDiff)) {
      // Check if green is reasonably stronger than red and blue
      if (greenFrequency < (redFrequency * 0.9) || greenFrequency < (blueFrequency * 0.9)) {
        return "GREEN";
      }
    }
    // Special case: if green is close to others but still lowest, it might still be green
    if (greenFrequency < redFrequency && greenFrequency < blueFrequency) {
      float greenAdvantage = ((float)(redFrequency + blueFrequency) / 2) / greenFrequency;
      if (greenAdvantage > 1.05) {  // Green is at least 5% stronger
        return "GREEN";
      }
    }
  }
  
  // RED detection
  if (redFrequency == minFreq && redFrequency < greenFrequency && redFrequency < blueFrequency) {
    if (redGreenDiff > minDifference && redBlueDiff > minDifference) {
      if (redFrequency < (greenFrequency * 0.8) && redFrequency < (blueFrequency * 0.8)) {
        return "RED";
      }
    }
  }
  
  // BLUE detection
  if (blueFrequency == minFreq && blueFrequency < redFrequency && blueFrequency < greenFrequency) {
    if (redBlueDiff > minDifference && greenBlueDiff > minDifference) {
      if (blueFrequency < (redFrequency * 0.8) && blueFrequency < (greenFrequency * 0.8)) {
        return "BLUE";
      }
    }
  }
  
  // Only use ratio detection if there's a significant color difference
  int maxDiff = max(redGreenDiff, max(redBlueDiff, greenBlueDiff));
  if (maxDiff > 20) {  // Only use ratios if there's a clear color difference
    float redRatio = (float)redFrequency / totalSignal;
    float greenRatio = (float)greenFrequency / totalSignal;
    float blueRatio = (float)blueFrequency / totalSignal;
    
    // Lower ratio means stronger color (for TCS3200)
    // Also check that the ratio difference is significant
    if (greenRatio < redRatio && greenRatio < blueRatio) {
      if ((redRatio - greenRatio) > 0.05 && (blueRatio - greenRatio) > 0.05) {
        return "GREEN (Ratio)";
      }
    }
    else if (redRatio < greenRatio && redRatio < blueRatio) {
      if ((greenRatio - redRatio) > 0.05 && (blueRatio - redRatio) > 0.05) {
        return "RED (Ratio)";
      }
    }
    else if (blueRatio < redRatio && blueRatio < greenRatio) {
      if ((redRatio - blueRatio) > 0.05 && (greenRatio - blueRatio) > 0.05) {
        return "BLUE (Ratio)";
      }
    }
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