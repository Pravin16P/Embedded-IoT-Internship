// Step 10: Read LDR (light) and HC-SR04 (distance)
// LDR module AO -> A1 | HC-SR04 Trig -> D9, Echo -> D10

const int ldrPin  = A1;
const int trigPin = 9;
const int echoPin = 10;

int readLight() {
  return analogRead(ldrPin);              // 0 - 1023
}

float readDistanceCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);            // 10 us trigger pulse
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);   // timeout 30 ms
  if (duration == 0) return -1;           // no echo received
  return duration * 0.0343 / 2.0;         // cm
}

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.println("Step 10: LDR + HC-SR04 started");
}

void loop() {
  int light = readLight();
  float distance = readDistanceCm();

  Serial.print("Light: ");
  Serial.print(light);

  Serial.print("  Distance: ");
  if (distance < 0) {
    Serial.println("out of range");
  } else {
    Serial.print(distance, 1);
    Serial.println(" cm");
  }

  delay(500);
}