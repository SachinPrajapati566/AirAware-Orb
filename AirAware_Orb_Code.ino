#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include "DHT.h"

// Definitions
#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ135PIN 34
#define LEDPIN 15
#define NUMPIXELS 60

#define I2C_ADDR 0x27
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
Adafruit_NeoPixel pixels(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  dht.begin();
  pixels.begin();
  lcd.init();
  lcd.backlight();
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int airQuality = analogRead(MQ135PIN);

  if (!isnan(temp) && !isnan(hum)) {
    // 1. First Output: AQI + Temp + Hum + RGB Color
    String airStatus;
    int r = 0, g = 0, b = 0;

    if (airQuality < 2000) {
      airStatus = "Good";
      r = 0; g = 255; b = 0;   // Green
    } else if (airQuality < 3000) {
      airStatus = "Moderate";
      r = 255; g = 255; b = 0; // Yellow
    } else {
      airStatus = "Poor";
      r = 255; g = 0; b = 0;   // Red
    }

    setColor(r, g, b);
    scrollText("Temp: " + String(temp, 1) + "C | Hum: " + String(hum, 0) + "% | AQ: " + String(airQuality) + " " + airStatus, r, g, b);

    delay(3000);

    // 2. Second Output: Environment Feasibility
    String feelStatus;
    int r2 = 0, g2 = 0, b2 = 0;

    if (temp >= 20 && temp <= 30 && hum >= 30 && hum <= 60 && airQuality < 2000) {
      feelStatus = "Feels Pleasant";
      r2 = 135; g2 = 206; b2 = 235; // Sky Blue
    } else {
      feelStatus = "Feels Suffocated";
      r2 = 255; g2 = 0; b2 = 255;   // Purple
    }

    setColor(r2, g2, b2);
    scrollText(feelStatus, r2, g2, b2);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DHT11 Error");
    setColor(255, 0, 0); // Red for error
  }

  delay(3000);
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

void scrollText(String text, uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < text.length() - 15; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(text.substring(i, i + 16));
    setColor(r, g, b);  // Reapply color while scrolling
    delay(300);
  }
}
