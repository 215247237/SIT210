#include <WiFiNINA.h>
#include <PubSubClient.h>   // MQTT library
#include <BH1750.h>         // BH1750 light sensor library
#include <Wire.h>

BH1750 lightMeter;

// WiFi Credentials
#define WIFI_SSID "XXXXXXXXXXXX" // REDACTED NETWORK NAME
#define WIFI_PASSWORD "XXXXXXXXXXXX" // REDACTED NETWORK PASSWORD

// MQTT Broker Credentials
#define MQTT_SERVER "XXXXXXXXXXXX.s1.eu.hivemq.cloud" // REDACTED HIVEMQ SERVER
#define MQTT_PORT 8883
#define MQTT_TOPIC "sensor/light"

#define MQTT_USER "hivemq.webclient.XXXXXXXXXXXX"  // REDACTED HIVEMQ USERNAME
#define MQTT_PASSWORD "XXXXXXXXXXXX"  // REDACTED HIVEMQ PASSWORD

const int sunlightThreshold = 50;
float lux = 0;
bool sunlightDetected = false;
bool previousSunlightReading = false;

// Use SSL Client instead of regular WiFiClient
WiFiSSLClient wifiSSLClient;
PubSubClient mqttClient(wifiSSLClient);

// Function to Connect to MQTT Broker
void connectMQTT()
{
    while (!mqttClient.connected())
    {
        Serial.println("Connecting to MQTT...");

        if (mqttClient.connect("ArduinoNanoIoT", MQTT_USER, MQTT_PASSWORD))
        {
            Serial.println("Connected to MQTT Broker!");
        } 
        else
        {
            Serial.print("Failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void getLightReading()
{
    lux = lightMeter.readLightLevel(); // Reads BH1750 light sensor

    // If sunlight is detected on terrarium
    if (lux >= sunlightThreshold)
    {
        sunlightDetected = true;
    }
    // If no sunlight is detected on terrarium
    else
    {
        sunlightDetected = false;
    }
}

void updateLightReading()
{
    getLightReading(); // Updates sunlight detection reading

    if (sunlightDetected != previousSunlightReading)
    {
        // If sunlight is detected on terrarium
        if (sunlightDetected)
        {
            Serial.println("Sunlight Detected!");
            mqttClient.publish(MQTT_TOPIC, "Sunlight detected");
        }
        // If no sunlight is detected on terrarium
        else
        {
            Serial.println("No sunlight detected");
            mqttClient.publish(MQTT_TOPIC, "No sunlight detected");
        }
    }
    
    // Updates light reading
    previousSunlightReading = sunlightDetected;
}

void setup()
{
    // BH1750 light sensor setup
    Serial.begin(9600);
    Wire.begin();
    lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23);

    // Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    
    Serial.println("Connected to WiFi");

    // Set MQTT Server
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  
    // Connect to MQTT
    connectMQTT();

    // Initialise light reading
    getLightReading();
    previousSunlightReading = sunlightDetected;
}

void loop()
{
    if (!mqttClient.connected())
    {
        connectMQTT();
    }

    // Keep MQTT connection alive
    mqttClient.loop(); 

    // Checks light readings and publishes to MQTT if changes detected
    updateLightReading();

    // Send data every 5 seconds
    delay(5000); 
}
