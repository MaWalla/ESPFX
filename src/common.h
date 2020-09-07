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
    void _gradient(void);
    void _streamline(void);
    void _random(void);
    void _randomLeadColor(void);
    void _randomLeadGradient(void);
    void _off(void);
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


void CommonLEDs::_gradient() {

}


void CommonLEDs::_streamline() {
  int offset = data["offset"].as<int>();
  int currentLength = data["current_length"].as<int>();
  for(int i=0; i<currentLength; i++) {
    leds[i + offset].setRGB(
      data["led_list"][i][0].as<int>(),
      data["led_list"][i][1].as<int>(),
      data["led_list"][i][2].as<int>()
    );
  }
}


void CommonLEDs::_random() {

}


void CommonLEDs::_randomLeadColor() {

}

void CommonLEDs::_randomLeadGradient() {

}

void CommonLEDs::_off() {
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(0, 0, 0);
  }
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
  } else if(mode == "random") {
    _random(); 
  } else if(mode == "off") {
    _off(); 
  } else if(mode == "gradient") {
    _gradient(); 
  } else if(mode == "randomLeadColor") {
    _randomLeadColor(); 
  } else if(mode == "randomLeadGradient") {
    _randomLeadGradient(); 
  }
}
