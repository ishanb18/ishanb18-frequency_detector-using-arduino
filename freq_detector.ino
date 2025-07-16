#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int micPin = A0;
const int potPin = A1;
const int led1Pin = 2;  // LED1: Delay status
const int led2Pin = 3;  // LED2: Device ON/OFF status (MAX BRIGHTNESS ONLY)
const int led3Pin = 5;  // LED3: Brightness based on frequency accuracy
const int crossingTarget = 20;
const int minAmplitude = 30;
const float correctionFactor = 0.94;

bool deviceState = false;
bool wasInRange = false;
float lastFrequency = 0;
bool readyToDetect = false;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.print("Initializing...");

  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(led3Pin, LOW);

  // Speed up ADC
  ADCSRA &= ~((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));
  ADCSRA |= (1 << ADPS2);

  delay(500);

  for (int i = 5; i > 0; i--) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Starting in: ");
    lcd.print(i);
    lcd.print(" sec");
    delay(1000);
  }

  readyToDetect = true;
  digitalWrite(led1Pin, HIGH);  // Turn on LED1 after countdown
  lcd.clear();
  lcd.print("Ready...");
  delay(1000);
}

void loop() {
  if (!readyToDetect) return;

  // Read and smooth potentiometer input
  long total = 0;
  const int samples = 10;

  for (int i = 0; i < samples; i++) {
    total += analogRead(potPin);
    delay(2);
  }

  int potValue = total / samples;
  float targetFreq = map(potValue, 0, 1023, 500, 5000);

  int crossings = 0;
  bool prevHigh = false;
  int minVal = 1023, maxVal = 0;
  unsigned long sampleStart = millis();

  while (millis() - sampleStart < 100) {
    int val = analogRead(micPin);
    minVal = min(minVal, val);
    maxVal = max(maxVal, val);
  }

  int threshold = (minVal + maxVal) / 2;

  if ((maxVal - minVal) < minAmplitude && !deviceState) {
    analogWrite(led3Pin, 0);
    digitalWrite(led2Pin, LOW);
    printStatus(targetFreq, 0, "No Signal");
    delay(300);
    return;
  }

  if ((maxVal - minVal) < minAmplitude && deviceState) {
    analogWrite(led3Pin, 0);
    digitalWrite(led2Pin, HIGH);
    printStatus(targetFreq, lastFrequency, "ON");
    delay(300);
    return;
  }

  unsigned long startTime = micros();
  while (crossings < crossingTarget) {
    int current = analogRead(micPin);
    bool currentHigh = current > threshold;

    if (currentHigh != prevHigh) {
      crossings++;
      prevHigh = currentHigh;
    }

    if (micros() - startTime > 1000000) {
      analogWrite(led3Pin, 0);
      digitalWrite(led2Pin, deviceState ? HIGH : LOW);
      printStatus(targetFreq, deviceState ? lastFrequency : 0, deviceState ? "ON" : "Too Slow");
      return;
    }
  }

  unsigned long duration = micros() - startTime;
  float freq = ((crossings / 2.0) / (duration / 1000000.0)) * correctionFactor;

  int brightness;
  float error = abs(freq - targetFreq);
  if (error <= 50) {
    brightness = 255;
  } else if (error <= 200) {
    brightness = 180;
  } else {
    brightness = 80;
  }

  analogWrite(led3Pin, brightness);

  bool inRange = error <= 100;
  if (inRange && !wasInRange) {
    deviceState = !deviceState;
  }

  wasInRange = inRange;
  lastFrequency = freq;

  digitalWrite(led2Pin, deviceState ? HIGH : LOW);
  printStatus(targetFreq, freq, deviceState ? "ON" : "OFF");

  delay(300);
}

void printStatus(float target, float measured, String status) {
  Serial.print("Target: ");
  Serial.print(target, 1);
  Serial.print(" Hz | Measured: ");
  Serial.print(measured, 1);
  Serial.print(" Hz | Status: ");
  Serial.println(status);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(target, 0);
  lcd.print("Hz F:");
  lcd.print(measured, 0);
  lcd.setCursor(0, 1);
  lcd.print("Status:");
  lcd.print(status);
}
