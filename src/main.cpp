#include <Arduino.h>
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>

#include <ArduinoJson.h>

const char* filename = "/config.json";

IPAddress IPAddress;

void setup() {
  Serial.begin(115200);
  SPIFFS.begin(true);

  File configFile = SPIFFS.open(filename);
  StaticJsonDocument<128> config;
  deserializeJson(config, configFile);

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

void loop() {

}