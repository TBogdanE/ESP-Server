#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>

// Replace with your network credentials
const char *ssid = "your_wifi_ssid";
const char *password = "your_wifi_password";

// Create a WebSocket server object on port 81
WebSocketsServer webSocket = WebSocketsServer(81);

bool waitForOK = false;
String jsonData = "";

// WebSocket event handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", num);
    break;
  case WStype_CONNECTED:
  {
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("[%u] Connected from %s\n", num, ip.toString().c_str());
    webSocket.sendTXT(num, "Connected now");
    break;
  }
  case WStype_TEXT:
  {
    // Serial.printf("[%u] Received text: %s\n", num, payload);
    Serial.printf("%s\n", payload);
    webSocket.sendTXT(num, "Message received");

    // Store the received JSON data
    jsonData = String((char *)payload);
    Serial.println("Received JSON data:");
    Serial.println(jsonData);

    // Send "NewTask" command to Arduino until we receive an "CommandOK" response
    waitForOK = true;
    while (waitForOK)
    {
      Serial.println("NewTask");
      delay(1000);
      if (Serial.available() > 0)
      {
        String response = Serial.readStringUntil('\n');
        if (response == "CommandOK")
        {
          waitForOK = false;
          // Send additional data to Arduino
          Serial.println(jsonData);
        }
      }
    }
    break;
  }
  default:
    break;
  }
}

void setup()
{
  // Start the Serial communication to communicate with Arduino
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server started");
}

void loop()
{
  webSocket.loop(); // Handle WebSocket events

  // Check if there is data available from the Arduino
  if (Serial.available() > 0)
  {
    String receivedFromArduino = Serial.readStringUntil('\n');
    Serial.println("Received from Arduino: " + receivedFromArduino);
    webSocket.broadcastTXT(receivedFromArduino.c_str()); // Broadcast received data over WebSocket
  }
}
