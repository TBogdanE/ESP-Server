#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>

// Replace with your network credentials
const char *ssid = "dlink-5390";
const char *password = "buvme97574";

// Create a WebSocket server object on port 81
WebSocketsServer webSocket = WebSocketsServer(81);

bool waitForOK = false;
String jsonData = "";

// WebSocket event handler
void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
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
    // Serial.printf("[%u] Received text: %s\n", num, payload);
    Serial.printf("%s", payload);
    webSocket.sendTXT(num, "Message received");

    // Store the received JSON data
    jsonData = String((char *)payload);
    Serial.println("Received JSON data: ");
    Serial.println(jsonData);

    // Send "newtask" command to Arduino until we receive an "ok" response
    waitForOK = true;
    while (waitForOK)
    {
      Serial.println("Sending newtask command to Arduino...");
      Serial.println("NewTask");
      // delay(1000);
      if (Serial.available() > 0)
      {
        String response = Serial.readStringUntil('\n');
        if (response == "CommandOK")
        {
          waitForOK = false;
          Serial.println("Received OK from Arduino. Sending JSON data...");
          Serial.println(jsonData);
        }
      }
    }
    break;
  }
}

void setup()
{
  // Start the Serial communication to send messages to the computer
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
  webSocket.onEvent(handleWebSocketEvent);
  Serial.println("WebSocket server started");
}

void loop()
{
  webSocket.loop(); // Handle WebSocket events
  // Check if there is data available from the Arduino
  if (Serial.available() > 0)
  {
    String recievedJSON = Serial.readStringUntil('\n');
    Serial.println("Received from Arduino: " + recievedJSON);
    webSocket.broadcastTXT(recievedJSON.c_str());
  }
}