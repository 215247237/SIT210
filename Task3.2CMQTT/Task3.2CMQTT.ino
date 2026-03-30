#include <WiFiNINA.h>
#include <PubSubClient.h>   // MQTT library
#include <HCSR04.h>         // HC-SR04 ultrasonic sensor library

#define WIFI_SSID "XXXXXXXXX"        // REDACTED NETWORK NAME
#define WIFI_PASSWORD "XXXXXXXXX"    // REDACTED NETWORK PASSWORD

// MQTT Broker Credentials
#define MQTT_SERVER "broker.emqx.io"
#define MQTT_PORT 1883

byte triggerPin = 16;
byte echoPin = 15;
const int ledPin = 10;
const int waveProximity = 12;
const int patProximity = 2;
int currentLedState = -1;
int lastPublishedLedState = -1;

// Creates a network client used by MQTT client for communication
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup () {
  Serial.begin(9600);
  HCSR04.begin(triggerPin, echoPin);
  pinMode(ledPin, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");

  // Defines MQTT broker and sets mqttCallback function to be called when message is received
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);

  connectMQTT();

  // Subscribe MQTT broker to topics
  mqttClient.subscribe("ES/Wave");
  mqttClient.subscribe("ES/Pat");
}

// Function to connect client to MQTT broker
void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");

    if (mqttClient.connect("ArduinoNanoIoT")) {
      Serial.println("Connected to MQTT Broker!");
    }
    // Retry connection if failed
    else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

// Function that handles chosen LED and its state
void ledState(int pin, int state) { 
  digitalWrite(pin, state);
}

// Function that returns HC-SR04 ultrasonic sensor distance reading in cm
double getDistance(){ 
  return HCSR04.measureDistanceCm()[0];
}

// Function to turn LEDs on if wave is detected
void wave() { 
  if (currentLedState != HIGH) {                    // Prevents spam
    ledState(ledPin, HIGH);                         
    Serial.println("Wave detected: Lights ON"); 
    currentLedState = HIGH;                         // Updates LED state
  } 
}

// Function to turn LEDs off if pat is detected
void pat() { 
  if (currentLedState != LOW) {                    // Prevents spam
    ledState(ledPin, LOW);
    Serial.println("Pat detected: Lights OFF"); 
    currentLedState = LOW;                         // Updates LED state
    delay(2000);                                   // Cooldown to prevent wave being detected immediately after pat
  } 
}

// Function that handles received message from broker
void mqttCallback(char *topic, byte *payload, unsigned int length) {
  // Prints topic to serial monitor
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  // Converts incoming payload to string
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Print converted payload to serial monitor
  Serial.print("Message: ");
  Serial.println(message);
  Serial.println("-----------------------");

  // If the received topic is ES/Wave, turn LEDs on
  if (String(topic) == "ES/Wave") {
    wave();
  }
  // If the received topic is ES/Pat, turn LEDs off
  else if (String(topic) == "ES/Pat") {
    pat();
  }
}

void loop () {
  // Ensures MQTT client remains connected to broker
  if (!mqttClient.connected()) {
    connectMQTT();
  }

  // Checks for incoming messages, handles MQTT packet, calls mqttCallback function
  mqttClient.loop();

  double distance = getDistance();
  // Serial.print(distance);
  // Serial.println(" cm");

  int currentDetectedLedState = -1;

  // If movement is detected <= 2cm, LEDs will be set to off
  if (distance <= patProximity) { 
    currentDetectedLedState = LOW;
  } 
  // If movement is detected <= 12cm, LEDs will be set to on
  else if (distance <= waveProximity) { 
    currentDetectedLedState = HIGH;
  }

  // Publish message to broker only if LED state is valid and has changed
  if (currentDetectedLedState != -1 && currentDetectedLedState != lastPublishedLedState) {
    if (currentDetectedLedState == LOW) {
      mqttClient.publish("ES/Pat", "Jacqui Rovira");        // Publish that LEDs are off (pat)
    }
    else {
      mqttClient.publish("ES/Wave", "Jacqui Rovira");       // Publish that LEDs are on (wave)
    }
    lastPublishedLedState = currentDetectedLedState;   // Updates last published state
  }
  delay(250);
}