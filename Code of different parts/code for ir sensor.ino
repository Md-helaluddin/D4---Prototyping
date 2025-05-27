// IR sensor pins
const int rightIR = A1;  // A0 = right IR sensor
const int leftIR  = A0;  // A1 = left IR sensor

void setup() {
  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);

  Serial.begin(9600);
}

void loop() {
  int left = digitalRead(leftIR);     // A1
  int right = digitalRead(rightIR);   // A0

  if (left == 0 && right == 0){
    Serial.println("None on line");
  }
  else if (left == 1 && right == 0){
    Serial.println("Left on line");
  }
  else if (left == 0 && right == 1){
    Serial.println("Right on line");
  }
  else if (left == 1 && right == 1){
    Serial.println("Both on line");
  }

  delay(1000); // slow down printing for readability
}
