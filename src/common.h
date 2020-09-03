#define FASTLED_ESP32_I2S true

#include <ArduinoJson.h>
#include <FastLED.h>

#ifndef _CommonLEDs_h
#define _CommonLEDs_h

class CommonLEDs {
  public:
    CommonLEDs();
    void display(char (&buffer)[BUFFERSIZE]);
  private:
    void _singleColor(void);
    void _streamline(void);
    void _random(void);
};

#endif


// AFAIK this bottom part should get its own file, named common.cpp
// but I'm just a stupid python dev new to this whole C++ thing.
// This should do for now :3


StaticJsonDocument<16384> data;
CRGB leds[NUM_LEDS];


CommonLEDs::CommonLEDs() {
  FastLED.addLeds<LED_CHIPSET, LEDPIN, LED_COLOR_ORDER>(leds, NUM_LEDS);
}



void CommonLEDs::_singleColor() {
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(
      data["input_color"][0].as<int>(),
      data["input_color"][1].as<int>(),
      data["input_color"][2].as<int>()
    );
  }
}


void CommonLEDs::_streamline() {
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(
      data["led_list"][i][0].as<int>(),
      data["led_list"][i][1].as<int>(),
      data["led_list"][i][2].as<int>()
    );
  }
}


void CommonLEDs::_random() {

}


void CommonLEDs::display(char (&buffer)[BUFFERSIZE]) {
  DeserializationError error = deserializeJson(data, buffer);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    Serial.println(buffer);
    return;
  }

  String mode = data["mode"].as<String>();

  if(mode == "single_color") {
    _singleColor();
  } else if(mode == "streamline") {
    _streamline(); 
  }
}
