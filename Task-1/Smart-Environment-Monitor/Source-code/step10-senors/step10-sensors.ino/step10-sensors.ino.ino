// Step 11: Smart Home Environment Monitoring System (final)
// Sensors: DHT22, LDR, HC-SR04 | Output: I2C LCD, buzzer, 2 LEDs, Serial log

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ---------- Pins ----------
#define DHTPIN   2
#define DHTTYPE  DHT22
const int buttonPin = 7;    // switch LCD page
const int buzzerPin = 8;
const int trigPin   = 9;
const int echoPin   = 10;
const int ledAlarm  = 11;   // red
const int ledOk     = 12;   // green
const int ldrPin    = A1;

// ---------- Alarm thresholds ----------
const float TEMP_MAX  = 35.0;   // C
const float HUM_MAX   = 80.0;   // %
const float DIST_MIN  = 10.0;   // cm
const int   LIGHT_LEVEL = 500;  // 0-1023
const bool  DARK_IS_HIGH = true; // Light setting: set false if value drops in darkness

// ---------- Objects ----------
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- Data ----------
struct SensorData {
  float temperature;
  float humidity;
  float distance;   // -1 = out of range
  int   light;
};

struct AlarmState {
  bool temp, hum, dist, dark;
};

SensorData data;
AlarmState alarms;
int page = 0;                       // 0 = values, 1 = status
unsigned long lastSample = 0;
int lastButton = HIGH;

// ---------- Sensor functions ----------
float readDistanceCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * 0.0343 / 2.0;
}

void readSensors() {
  data.temperature = dht.readTemperature();
  data.humidity    = dht.readHumidity();
  data.light       = analogRead(ldrPin);
  data.distance    = readDistanceCm();
}

// ---------- Alarm logic ----------
void checkAlarms() {
  alarms.temp = !isnan(data.temperature) && data.temperature > TEMP_MAX;
  alarms.hum  = !isnan(data.humidity)    && data.humidity > HUM_MAX;
  alarms.dist = data.distance >= 0       && data.distance < DIST_MIN;
  alarms.dark = DARK_IS_HIGH ? (data.light > LIGHT_LEVEL)
                             : (data.light < LIGHT_LEVEL);
}

void updateOutputs() {
  bool loud = alarms.temp || alarms.hum || alarms.dist;
  bool any  = loud || alarms.dark;

  digitalWrite(ledAlarm, any ? HIGH : LOW);
  digitalWrite(ledOk,    any ? LOW  : HIGH);

  if (loud) tone(buzzerPin, 1000);
  else      noTone(buzzerPin);
}

// ---------- Display ----------
void updateLCD() {
  lcd.clear();
  if (page == 0) {
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(data.temperature, 1);
    lcd.print("C H:");
    lcd.print(data.humidity, 0);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("L:");
    lcd.print(data.light);
    lcd.print(" D:");
    if (data.distance < 0) lcd.print("--");
    else { lcd.print(data.distance, 0); lcd.print("cm"); }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Status:");
    lcd.setCursor(0, 1);
    bool any = alarms.temp || alarms.hum || alarms.dist || alarms.dark;
    if (!any) {
      lcd.print("All normal");
    } else {
      lcd.print("ALARM ");
      if (alarms.temp) lcd.print("T ");
      if (alarms.hum)  lcd.print("H ");
      if (alarms.dist) lcd.print("D ");
      if (alarms.dark) lcd.print("DK");
    }
  }
}

// ---------- Serial log (CSV) ----------
void logSerial() {
  Serial.print(millis() / 1000);   Serial.print(",");
  Serial.print(data.temperature, 1); Serial.print(",");
  Serial.print(data.humidity, 1);    Serial.print(",");
  Serial.print(data.light);          Serial.print(",");
  Serial.print(data.distance, 1);    Serial.print(",");
  bool any = alarms.temp || alarms.hum || alarms.dist || alarms.dark;
  if (!any) Serial.println("OK");
  else {
    if (alarms.temp) Serial.print("TEMP ");
    if (alarms.hum)  Serial.print("HUM ");
    if (alarms.dist) Serial.print("DIST ");
    if (alarms.dark) Serial.print("DARK");
    Serial.println();
  }
}

// ---------- Button ----------
void checkButton() {
  int b = digitalRead(buttonPin);
  if (b == LOW && lastButton == HIGH) {
    page = !page;
    updateLCD();
    delay(20);                      // debounce
  }
  lastButton = b;
}

// ---------- Main ----------
void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledAlarm, OUTPUT);
  pinMode(ledOk, OUTPUT);

  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.print("Smart Home Mon.");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);

  Serial.println("time_s,temp_C,humidity_pct,light,distance_cm,alarm");
}

void loop() {
  checkButton();

  if (millis() - lastSample >= 2000) {   // DHT22 needs >= 2 s
    lastSample = millis();
    readSensors();
    checkAlarms();
    updateOutputs();
    updateLCD();
    logSerial();
  }
}