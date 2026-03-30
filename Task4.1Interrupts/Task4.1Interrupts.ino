#include <BH1750.h>       // BH1750 light sensor library
#include <Wire.h>

BH1750 lightMeter;

const int buttonInterruptPin = 2;    // Valid interrupt pin for Arduino Nano
const int ledGreenPin = 8;
const int ledBluePin = 6;
const int pirSensorPin = 14;
const int luxThreshold = 50;
int ledState = LOW;
int buttonState = HIGH;
int motionState = LOW;
int lux = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23);   // Configures BH1750 light sensor

  pinMode(ledGreenPin, OUTPUT);                 // Configures led 1
  pinMode(ledBluePin, OUTPUT);                  // Configures led 2
  pinMode(buttonInterruptPin, INPUT_PULLUP);    // Configures button
  pinMode(pirSensorPin, INPUT);                 // Configures HC-SR501 PIR motion sensor

  attachInterrupt(digitalPinToInterrupt(buttonInterruptPin), toggle, FALLING);    // Configures interrupt to call toggle() function when button is pressed
}

// Function to turn on given LED
void turnOnLed(int ledPin) {
  digitalWrite(ledPin, HIGH);
}

// Function to run given LED for a specified time before turning off
void runLed(int ledPin, int delayDuration) {
  delay(delayDuration);
  digitalWrite(ledPin, LOW);
}

// Function handling Leds' sequences
void houseLighting(int ledPin1, int ledPin2, int delayDuration1, int delayDuration2) {
  turnOnLed(ledPin1);
  turnOnLed(ledPin2);
  runLed(ledPin1, delayDuration1);
  runLed(ledPin2, delayDuration2);
}

// Function to get the current reading from the BH1750 light sensor
double getLux() {
  return lightMeter.readLightLevel();
}

// Function to get the current reading from the HC-SR501 PIR motion sensor
int getMotionState() {
  return digitalRead(pirSensorPin);
}

void loop() {
  motionState = getMotionState();
  lux = getLux();

  if ((motionState == HIGH && lux <= luxThreshold)) { // If motion is detected AND area is dark enough
    Serial.print("Low brightness (");
    Serial.print(lux);
    Serial.println(" lux) and motion detected. Lights ON");
    houseLighting(ledGreenPin, ledBluePin, 30000, 30000);                
    Serial.println("Time expired. Lights OFF");
  }
}

// Function handling interrupt when button is pressed
void toggle() {
  ledState = !ledState;                   
  digitalWrite(ledGreenPin, ledState);  // Toggle led state on/off
  digitalWrite(ledBluePin, ledState);

  if (ledState == HIGH) {
    Serial.println("Button interrupt detected. Lights ON");
  }
  else {
    Serial.println("Button interrupt detected. Lights OFF");
  }
}