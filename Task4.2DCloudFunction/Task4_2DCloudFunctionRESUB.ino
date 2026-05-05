#include <WiFiNINA.h>         // Wifi library
#include <PubSubClient.h>     // RESUB: MQTT library

// WiFiServer server(80);        // RESUB: Removed server to listen on port 80

// WiFi Credentials
#define WIFI_SSID "XXXXXXXXX"        // REDACTED NETWORK NAME
#define WIFI_PASSWORD "XXXXXXXXX"    // REDACTED NETWORK PASSWORD

// RESUB: MQTT Broker Credentials
#define MQTT_SERVER "broker.emqx.io"
#define MQTT_PORT 1883

// RESUB: Creates network client used by MQTT client for communication
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const int ledBlue = 16;       // Living room
const int ledYellow = 15;     // Bathroom
const int ledRed = 14;        // Closet         
int ledBlueState = 0;         // Default LED state is off
int ledYellowState = 0;
int ledRedState = 0;

// RESUB: Added callback function to handle incoming MQTT messages and control LEDs
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (message == "livingroom") {
    toggleLed("livingroom");
    printLedState("Living room", ledBlueState);
  }
  else if (message == "bathroom") {
    toggleLed("bathroom");
    printLedState("Bathroom", ledYellowState);
  }
  else if (message == "closet") {
    toggleLed("closet");
    printLedState("Closet", ledRedState);
  }
}

void setup() {
  Serial.begin(9600);

  // Configure LEDs
  pinMode(ledBlue, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledRed, OUTPUT);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // RESUB: Added MQTT setup
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(callback);

  connectMQTT();

  // server.begin();       // RESUB: Removed server listening on port 80
}

// Function that handles specified room's light toggling
void toggleLed(String room) {
  if (room == "livingroom") {
    ledBlueState = !ledBlueState;
    digitalWrite(ledBlue, ledBlueState);
  }
  else if (room == "bathroom") {
    ledYellowState = !ledYellowState;
    digitalWrite(ledYellow, ledYellowState);
  }
  else if (room == "closet") {
    ledRedState = !ledRedState;
    digitalWrite(ledRed, ledRedState);
  }
}

// Function that handles printing specified room's current light state to serial monitor
void printLedState(String room, int ledState) {
  Serial.print(room);
  Serial.print(" light ");

  if (ledState == HIGH) {
    Serial.println("ON");
  }
  else {
    Serial.println("OFF");
  }
}

// RESUB: Function that handles MQTT connection
void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");

    if (mqttClient.connect("ArduinoNanoIoT")) {
      Serial.println("Connected to MQTT Broker!");
      mqttClient.subscribe("lindas_lights");    // Subscribe MQTT broker to topic
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

// RESUB: Updated to maintain connection with MQTT broker and handle messages via callback()
void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }

  mqttClient.loop();
}

