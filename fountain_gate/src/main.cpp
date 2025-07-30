#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal_74HC595.h>

// DHT11 sensor configuration
#define DHTPIN 2 
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Control pins for 74HC595 shift register
const int dataPin  = 8;   // DS (Serial Data Input)
const int latchPin = 9;   // ST_CP (Storage Register Clock)
const int clockPin = 10;  // SH_CP (Shift Register Clock)

// Humidifier and button pins
const int button = 12;
const int humidifer = 11;

// Timing control for temperature and humidity updates
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 2000; // in milliseconds

// LCD pin mapping to 74HC595 (Q0 to Q5): RS, E, D4, D5, D6, D7
LiquidCrystal_74HC595 lcd(dataPin, clockPin, latchPin, 4, 5, 0, 1, 2, 3);

// Simulates a capacitive touch to toggle the humidifier
void humidifier_start() {
  digitalWrite(humidifer, HIGH);
  delay(300);
  digitalWrite(humidifer, LOW);
}

void setup() {
  Serial.begin(9600);
  dht.begin();

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Hello, World!");

  // Input setup
  pinMode(button, INPUT_PULLUP);  // Button to manually toggle humidifier

  // Output setup
  digitalWrite(humidifer, HIGH);  // Initialize humidifier in "off" state
  pinMode(humidifer, OUTPUT);
}

bool humidifierIsOn = false;

void loop() {
  // Read touch sensor (or simulated signal) from analog input A0
  int touchState = analogRead(A0);

  unsigned long now = millis();

  // Update temperature and humidity every 2 seconds
  if (now - lastUpdateTime >= updateInterval) {
    lastUpdateTime = now;

    float temp = dht.readTemperature();
    float humi = dht.readHumidity();

    Serial.print("Touch value = ");
    Serial.print(touchState);
    Serial.print(" | Humidity = ");
    Serial.println(humi);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temp);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("H:");
    lcd.print(humi);
    lcd.print("%");

    delay(100);

    // Turn ON the humidifier if humidity is below 60 and current state is OFF
    // touchState > 0 means humidifier is currently OFF
    if (humi < 60 && touchState > 0) {
      Serial.print("Turning ON | Touch = ");
      Serial.println(touchState);
      Serial.print("Humidity = ");
      Serial.println(humi);
      humidifier_start();  // Trigger the "touch" to turn on
      delay(2000);
    }

    delay(100);

    // Turn OFF the humidifier if humidity is >= 65 and currently ON
    // touchState == 0 means humidifier is currently ON
    if (humi >= 65 && touchState == 0) {
      Serial.print("Turning OFF | Touch = ");
      Serial.println(touchState);
      Serial.print("Humidity = ");
      Serial.println(humi);
      humidifier_start();  // Trigger the "touch" to turn off
      delay(2000);
    }
  }
}
