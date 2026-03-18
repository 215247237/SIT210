#include <WiFiNINA.h>
#include <PubSubClient.h>  // MQTT library

// WiFi Credentials
#define WIFI_SSID "XXXXXX" // REDACTED NETWORK NAME
#define WIFI_PASSWORD "XXXXXXX" // REDACTED NETWORK PASSWORD

// MQTT Broker Credentials
#define MQTT_SERVER "XXXXXXXXXX.s1.eu.hivemq.cloud" // REDACTED HIVEMQ SERVER
#define MQTT_PORT 8883
#define MQTT_TOPIC "sensor/motion"

#define MQTT_USER "hivemq.webclient.XXXXXXXXX"  // REDACTED HIVEMQ USERNAME
#define MQTT_PASSWORD "XXXXXXXX"  // REDACTED HIVEMQ PASSWORD

// Motion Sensor Pin
const int pirSensorPin = 14;
int motionDetected = 0;

// Use SSL Client instead of regular WiFiClient
WiFiSSLClient wifiSSLClient;
PubSubClient mqttClient(wifiSSLClient);

// Function to Connect to MQTT Broker
void connectMQTT() {
   while (!mqttClient.connected()) {
       Serial.println("Connecting to MQTT...");
       if (mqttClient.connect("ArduinoNanoIoT", MQTT_USER, MQTT_PASSWORD)) {
           Serial.println("Connected to MQTT Broker!");
       } else {
           Serial.print("Failed, rc=");
           Serial.print(mqttClient.state());
           Serial.println(" Retrying in 5 seconds...");
           delay(5000);
       }
   }
}

void setup() {
   Serial.begin(115200);
   pinMode(pirSensorPin, INPUT);

   // Connect to WiFi
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
   while (WiFi.status() != WL_CONNECTED) {
       delay(1000);
       Serial.println("Connecting to WiFi...");
   }
   Serial.println("Connected to WiFi");

   // Set MQTT Server
   mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  
   // Connect to MQTT
   connectMQTT();
}

void loop() {
   if (!mqttClient.connected()) {
       connectMQTT();
 }
   mqttClient.loop(); // Keep MQTT connection alive

   motionDetected = digitalRead(pirSensorPin); // Reads motion sensor

   // If motion is detected
   if (motionDetected == HIGH) {
       Serial.println("Motion Detected!");
       mqttClient.publish(MQTT_TOPIC, "Motion detected");
   }
   // If no motion is detected
   else {
       Serial.println("No motion");
       mqttClient.publish(MQTT_TOPIC, "No motion detected");
   }

   delay(5000); // Send data every 5 seconds
}
