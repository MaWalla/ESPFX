#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <SPIFFS.h>

#define FASTLED_ESP32_I2S true

#include <FastLED.h>
#include <ArduinoJson.h>

#define PIN         15
#define COLOR_ORDER GRB
#define NUM_LEDS    120
#define CHIPSET     WS2812B
#define UDPPORT     13321
#define BUFFERSIZE  6144

StaticJsonDocument<BUFFERSIZE> data;
StaticJsonDocument<128> config;
CRGB leds[NUM_LEDS];

const char* filename = "/config.json";
IPAddress IPAddress;

WiFiUDP udp;
char buffer[BUFFERSIZE];

void setup() {
  Serial.begin(115200);
  SPIFFS.begin(true);
  
  FastLED.addLeds<CHIPSET, PIN, COLOR_ORDER>(leds, NUM_LEDS);

  File configFile = SPIFFS.open(filename);
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

  udp.begin(UDPPORT);
  Serial.println("UDP Socket online");
}

void loop() {
  udp.parsePacket();
  if(udp.read(buffer, BUFFERSIZE) > 0) {
    DeserializationError error = deserializeJson(data, buffer);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      Serial.println(buffer);
      return;
    }
    for(int i=0; i<NUM_LEDS; i++) {
      leds[i].setRGB(data[i][0].as<int>(), data[i][1].as<int>(), data[i][2].as<int>());
    }
  }
  memset(buffer, 0, sizeof(buffer));
  FastLED.show();
}
