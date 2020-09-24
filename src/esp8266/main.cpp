#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "LittleFS.h"
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
  LittleFS.begin();

  File configFile = LittleFS.open(filename, "r");
  deserializeJson(config, configFile);

  connectWiFi();

  udp.begin(UDPPORT);
  Serial.println("UDP Socket online");
}


void loop() {
  udp.parsePacket();
  if(udp.read(buffer, BUFFERSIZE) > 0) {
    //int time = millis();
    commonleds.display(buffer);
    FastLED.show();
    //Serial.println(millis() - time);
  }
  memset(buffer, 0, sizeof(buffer));
}
