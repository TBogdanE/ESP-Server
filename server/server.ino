#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <SoftwareSerial.h>

// add here your network id and password
const char *ssid = "dlink-5390";
const char *password = "buvme97574";

// create a WebSocket server on port 81
WebSocketsServer webSocket = WebSocketsServer(81);

bool waitForOK = false;
String jsonData = "";

// webSocket event handler
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
  // recieve data from phone
  case WStype_TEXT:
    Serial.printf("%s", payload);
    // send to the phone confirmation
    webSocket.sendTXT(num, "Message received");

    // store the received JSON data
    jsonData = String((char *)payload);
    Serial.println("Received JSON data: ");
    Serial.println(jsonData);

    // !!! we cannot send directly the command to arduino, when recieving data from the phone;
    // we have to send it only when arduino is waiting for data on serial
    // we implement a way, so, when we are receiving data from phone, we send 'NewTask' to arduino continously;
    // when arduino reads from serial, checks if it's recieving 'NewTask', if yes, then it will stop it's loop, and send
    // back to esp 'CommandOk'; after ESP recieves 'CommandOk', it will send the command from the phone, than, continues to wait for new data
    waitForOK = true;
    while (waitForOK)
    {
      Serial.println("NewTask");
      delay(200);
      if (Serial.available() > 0)
      {
        String response = Serial.readStringUntil('\n');
        response.trim();
        if (response == "CommandOK")
        {
          delay(500);
          waitForOK = false;
          Serial.println(jsonData);
          delay(300);
        }
      }
    }
    break;
  }
}

void setup()
{
  // start the Serial communication to send messages to the computer
  Serial.begin(9600);

  // connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // start WebSocket server
  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);
  Serial.println("WebSocket server started");
}

void loop()
{
  webSocket.loop(); // handle WebSocket events
  // check if there is data available from the Arduino
  if (Serial.available() > 0)
  {
    String recievedJSON = Serial.readStringUntil('\n');
    Serial.println("Received from Arduino: " + recievedJSON);
    webSocket.broadcastTXT(recievedJSON.c_str());
  }
}