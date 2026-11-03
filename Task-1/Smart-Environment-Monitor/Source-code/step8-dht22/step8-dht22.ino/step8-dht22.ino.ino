// Step 9: Show DHT22 readings on a 16x2 I2C LCD
// DHT22 data -> D2 | LCD SDA -> A4 | LCD SCL -> A5

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN  2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);   // address 0x27, 16 columns, 2 rows

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Home Mon.");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);
  lcd.clear();
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity    = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor error    ");
    Serial.println("Error: DHT22 read failed");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature, 1);
    lcd.print((char)223);        // degree symbol
    lcd.print("C  ");

    lcd.setCursor(0, 1);
    lcd.print("Hum : ");
    lcd.print(humidity, 1);
    lcd.print(" %  ");

    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.print(" C  Humidity: ");
    Serial.print(humidity, 1);
    Serial.println(" %");
  }

  delay(2000);
}