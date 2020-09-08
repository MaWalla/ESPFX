#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#include "../common.h"


StaticJsonDocument<128> config;
const char* filename = "/config.json";
IPAddress IPAddress;
WiFiUDP udp;
char buffer[BUFFERSIZE];
CommonLEDs commonleds;


void connectWiFi() {
  const char* ssid = config["ssid"];
  const char* password = config["pass"];

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  IPAddress = WiFi.localIP();

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(IPAddress);
}


void setup() {
  Serial.begin(115200);
  SPIFFS.begin(true);

  File configFile = SPIFFS.open(filename);
  deserializeJson(config, configFile);

  connectWiFi();

  udp.begin(UDPPORT);
  Serial.println("UDP Socket online");
}


void loop() {
  udp.parsePacket();
  if(udp.read(buffer, BUFFERSIZE) > 0) {
    commonleds.display(buffer);
    FastLED.show();
  }
  memset(buffer, 0, sizeof(buffer));
}
