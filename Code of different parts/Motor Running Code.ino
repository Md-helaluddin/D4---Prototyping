// Motor control pins
const int mr1 = 2;  // Left motor IN1
const int mr2 = 3;  // Left motor IN2
const int ml1 = 4;  // Right motor IN1
const int ml2 = 5;  // Right motor IN2

void setup() {
  // Set all motor pins as outputs
  pinMode(mr1, OUTPUT);
  pinMode(mr2, OUTPUT);
  pinMode(ml1, OUTPUT);
  pinMode(ml2, OUTPUT);

  // Left motor forward (wires are reversed, so logic flipped)
  digitalWrite(mr1, LOW);  
  digitalWrite(mr2, HIGH);

  // Right motor forward
  digitalWrite(ml1, HIGH); 
  digitalWrite(ml2, LOW);
}

void loop() {
  // Motors keep running forward
}
