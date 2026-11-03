// Steps 5, 6, 7: Blink LED + Push Button + Analog Sensor
// Pin 12 -> LED (blink) | Pin 11 -> LED (button) | Pin 7 -> button | A0 -> potentiometer

const int ledBlink  = 12;   // Step 5: blinking LED
const int ledButton = 11;   // Step 6: LED controlled by button
const int buttonPin = 7;    // Step 6: push button
const int sensorPin = A0;   // Step 7: analog sensor

unsigned long lastBlink  = 0;
unsigned long lastSensor = 0;
bool blinkState = false;
int  lastButton = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(ledBlink, OUTPUT);
  pinMode(ledButton, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);   // pressed = LOW, released = HIGH
  Serial.println("Steps 5-7 started");
}

void loop() {
  // ---- Step 5: Blink LED every 1 s (non-blocking) ----
  if (millis() - lastBlink >= 1000) {
    lastBlink = millis();
    blinkState = !blinkState;
    digitalWrite(ledBlink, blinkState);
  }

  // ---- Step 6: Read push button ----
  int button = digitalRead(buttonPin);
  digitalWrite(ledButton, button == LOW ? HIGH : LOW);
  if (button != lastButton) {          // print only when state changes
    Serial.println(button == LOW ? "Button: PRESSED" : "Button: RELEASED");
    lastButton = button;
    delay(20);                         // simple debounce
  }

  // ---- Step 7: Read analog sensor every 500 ms ----
  if (millis() - lastSensor >= 500) {
    lastSensor = millis();
    int value = analogRead(sensorPin);          // 0 - 1023
    float volts = value * 5.0 / 1023.0;
    Serial.print("Analog value: ");
    Serial.print(value);
    Serial.print("  Voltage: ");
    Serial.print(volts, 2);
    Serial.println(" V");
  }
}