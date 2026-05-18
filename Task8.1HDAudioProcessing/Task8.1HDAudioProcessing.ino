#include <BH1750.h>       // BH1750 light sensor library
#include <Wire.h>         // I2C communication library for light sensor
#include <ArduinoBLE.h>   // Bluetooth library

BH1750 lightMeter;

const int ledGreenPin = 8;
const int ledBluePin = 9;
const int ledWhitePin = 10;
const int luxThreshold = 50;

bool bathroomLightActive = false;
unsigned long bathroomStartTime = 0;
const unsigned long bathroomDuration = 30000;

bool hallwayLightActive = false;
unsigned long hallwayStartTime = 0;
const unsigned long hallwayDuration = 60000;

bool fanActive = false;
unsigned long fanStartTime = 0;
const unsigned long fanDuration = 30000;

BLEService lindasLights("12345678-1234-1234-1234-123456789012");                              // Creates BLE service
BLEStringCharacteristic voiceCommands("88888888-4444-4444-4444-121212121212", BLEWrite, 50);  // Creates BLE characteristic

void setup() {
  Serial.begin(9600);
  Wire.begin();

  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23);   // Configures BH1750 light sensor

  pinMode(ledGreenPin, OUTPUT);                 // Configures bathroom LED
  pinMode(ledBluePin, OUTPUT);                  // Configures hallway LED
  pinMode(ledWhitePin, OUTPUT);                 // Configures fan LED

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed");

    while (1) {}
  }

  // Bluetooth setup
  BLE.setLocalName("LindaLights");
  BLE.setAdvertisedService(lindasLights);
  lindasLights.addCharacteristic(voiceCommands);
  BLE.addService(lindasLights);
  BLE.advertise();
  Serial.println("BLE peripheral device active");
}

// Function to get the current reading from the BH1750 light sensor
double getLux() {
  return lightMeter.readLightLevel();
}

// Function to determine if room is dark enough for lights to activate
bool roomDarkEnough() {
  double lux = getLux();

  if (lux <= luxThreshold) {
    return true;
  }

  return false;
}

// Function to turn exhaust fan on and start duration timer
void turnOnFan(int fanPin, bool &fanFlag, unsigned long &fanStartTime) {
  digitalWrite(fanPin, HIGH);

  fanFlag = true;
  fanStartTime = millis();
}

// Function to turn exhaust fan off after duration expires
void runFan(int fanPin, bool &fanFlag, unsigned long &fanStartTime, unsigned long fanDuration, String fanName) {
  if (fanFlag && millis() - fanStartTime >= fanDuration) {
    digitalWrite(fanPin, LOW);

    fanFlag = false;

    Serial.println("\nTime expired. " + fanName + " OFF");
  }
}

// Function to turn light on and start duration timer
void turnOnLight(int ledPin, bool &roomFlag, unsigned long &lightStartTime) {
  digitalWrite(ledPin, HIGH);

  roomFlag = true;
  lightStartTime = millis();
}

// Function to turn light off after duration expires
void runLight(int ledPin, bool &roomFlag, unsigned long &lightStartTime, unsigned long lightDuration, String lightName) {
  if (roomFlag && millis() - lightStartTime >= lightDuration) {
    digitalWrite(ledPin, LOW);

    roomFlag = false;

    Serial.println("\nTime expired. " + lightName + " OFF");
  }
}

// Function to print confirmation of light or fan activation
void printDeviceActivation(String deviceName, String command, bool lowLightRequired) {
  double lux = getLux();

  Serial.print("\nVoice command '");
  Serial.print(command);
  Serial.println("' registered");

  if (lowLightRequired) {
    Serial.print("Room brightness at ");
    Serial.print(lux);
    Serial.println(" lux");
  }

  Serial.println(deviceName + " ON");
}

// Function to handle received Bluetooth commands and turn on appropriate light or fan
void voiceActivation() {
  BLEDevice mic = BLE.central();    // Establish connection with Bluetooth device

  if (mic) {
    while (mic.connected()) {       // While Bluetooth is connected
      BLE.poll();
      
      if (voiceCommands.written()) {
        String command = voiceCommands.value();

        if (command == "BATHROOM_ON" && roomDarkEnough()) {                   // If received command is for bathroom and room is dark enough, turn on bathroom light and start timer
          printDeviceActivation("Bathroom light", "Bathroom", true);
          turnOnLight(ledGreenPin, bathroomLightActive, bathroomStartTime);
        }
        else if (command == "HALLWAY_ON" && roomDarkEnough()) {               // If received command is for hallway and room is dark enough, turn on hallway light and start timer
          printDeviceActivation("Hallway light", "Hallway", true);
          turnOnLight(ledBluePin, hallwayLightActive, hallwayStartTime);
        }
        else if (command == "FAN_ON") {                                       // If received command is for fan, turn on exhaust fan and start timer
          printDeviceActivation("Exhaust fan", "Exhaust", false);
          turnOnFan(ledWhitePin, fanActive, fanStartTime);
        }
      }
    }
  }
}

void loop() {
  voiceActivation();  // Handle received Bluetooth commands

  runLight(ledGreenPin, bathroomLightActive, bathroomStartTime, bathroomDuration, "Bathroom light");  // Turn off bathroom light after timer expires

  runLight(ledBluePin, hallwayLightActive, hallwayStartTime, hallwayDuration, "Hallway light");       // Turn off hallway light after timer expires

  runFan(ledWhitePin, fanActive, fanStartTime, fanDuration, "Exhaust fan");                           // Turn off exhaust fan after timer expires
}