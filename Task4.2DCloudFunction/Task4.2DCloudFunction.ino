#include <WiFiNINA.h>         // Wifi library

WiFiServer server(80);        // Creates server to listen on port 80

// WiFi Credentials
#define WIFI_SSID "XXXXXXXXXX"        // REDACTED NETWORK NAME
#define WIFI_PASSWORD "XXXXXXXXXX"    // REDACTED NETWORK PASSWORD

const int ledBlue = 16;       // Living room
const int ledYellow = 15;     // Bathroom
const int ledRed = 14;        // Closet         
int ledBlueState = 0;         // Default LED state is off
int ledYellowState = 0;
int ledRedState = 0;

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

  server.begin();       // Starts server listening on port 80
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

void loop() {
  WiFiClient browserClient = server.available();        // Checks for requests on port 80. Creates new client if request found

  if (browserClient) {
    // Serial.println("Connected to client");
    String receivedRequest = browserClient.readStringUntil('\r');     // Reads client's request for which light to turn on

    if (receivedRequest.indexOf("livingroom") != -1) {        // If packet contains "livingroom"
      toggleLed("livingroom");
      printLedState("Living room", ledBlueState);
    }
    else if (receivedRequest.indexOf("bathroom") != -1) {     // If packet contains "bathroom"
      toggleLed("bathroom");
      printLedState("Bathroom", ledYellowState);
    }
    else if (receivedRequest.indexOf("closet") != -1) {       // If packet contains "closet"
      toggleLed("closet");
      printLedState("Closet", ledRedState);
    }

    // Send response back to client
    browserClient.println("HTTP/1.1 200 OK");
    browserClient.println("Content-type:text/html");
    browserClient.println();

    browserClient.stop();  // Closes server connection
    // Serial.println("Disconnected from client");
  }
}
