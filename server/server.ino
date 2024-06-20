#include <ESP8266WiFi.h>
#include <SocketIOclient.h>
#include <WebSockets.h>
#include <WebSockets4WebServer.h>
#include <WebSocketsClient.h>
#include <WebSocketsServer.h>
#include <WebSocketsVersion.h>

const char *ssid = "SolarSystem Wi-Fi";
const char *password = "12344321";
IPAddress local_IP(192, 168, 4, 22);
IPAddress gateway(192, 168, 4, 9);
IPAddress subnet(255, 255, 255, 0);

WebSocketsServer webSocket = WebSocketsServer(81);

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
    webSocket.sendTXT(num, "Connected");
  }
  break;
  case WStype_TEXT:
    Serial.printf("[%u] Received text: %s\n", num, payload);
    webSocket.sendTXT(num, "Message received");
    break;
  }
}

void setup()
{
  Serial.begin(9600);
  delay(10);

  Serial.println();
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");
  // WiFi.softAP(ssid);
  // WiFi.softAP(ssid, password, channel, hidden, max_connection)

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);
  Serial.println('Websocket server started');
}
void loop()
{
  Serial.print("[Server Connected] ");
  /*Serial.println(WiFi.softAPIP());*/
  webSocket.loop();
  delay(500);
}