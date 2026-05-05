#include <BH1750.h>       // BH1750 light sensor library
#include <Wire.h>

BH1750 lightMeter;

const int buttonInterruptPin = 2;    // Valid interrupt pin for Arduino Nano
const int ledGreenPin = 8;
const int ledBluePin = 6;
const int pirSensorPin = 3;          // RESUB: Updated PIR sensor pin to interrupt pin
const int luxThreshold = 50;
int ledState = LOW;
// int buttonState = HIGH;          // RESUB: Removed as updated ISR and loop() logic handles states
// int motionState = LOW;
int lux = 0;
volatile bool buttonPressed = false;    // RESUB: Added variables for ISR. Volatile type alerts compiler that values can change at any time
volatile bool motionDetected = false;   

void setup() {
  Serial.begin(9600);
  Wire.begin();

  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23);   // Configures BH1750 light sensor

  pinMode(ledGreenPin, OUTPUT);                 // Configures led 1
  pinMode(ledBluePin, OUTPUT);                  // Configures led 2
  pinMode(buttonInterruptPin, INPUT_PULLUP);    // Configures button
  pinMode(pirSensorPin, INPUT);                 // Configures HC-SR501 PIR motion sensor

  attachInterrupt(digitalPinToInterrupt(buttonInterruptPin), buttonISR, FALLING);    // Configures interrupt to call toggleLedISR() function when button is pressed
  attachInterrupt(digitalPinToInterrupt(pirSensorPin), pirISR, RISING);              // RESUB: Configures interrupt to call pirISR() function when motion is detected
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
// RESUB: Function removed. Action updated to be handled by ISR
// int getMotionState() {
  // return digitalRead(pirSensorPin);
// }

void loop() {
  // RESUB: Motion flag used to trigger LED logic in loop()
  if (motionDetected) {
    motionDetected = false; // RESUB: Reset flag
    lux = getLux();

    if (lux <= luxThreshold) { // If area is dark enough
      Serial.print("Low brightness (");
      Serial.print(lux);
      Serial.println(" lux) and motion detected. Lights ON");
      houseLighting(ledGreenPin, ledBluePin, 3000, 3000);                
      Serial.println("Time expired. Lights OFF");
    }
  }

  // RESUB: Button press logic moved from ISR
  if (buttonPressed) {
    buttonPressed = false; // RESUB: Reset flag

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
}

// Function handling interrupt when button is pressed
// RESUB: Core logic moved to loop() and replaced with simplified trigger
void buttonISR() {
  buttonPressed = true;
}

// RESUB: Function handling interrupt when motion is detected
void pirISR() {
  motionDetected = true;
}