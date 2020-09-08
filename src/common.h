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
    void _singleColor(float brightness);
    void _gradient(float brightness);
    void _streamline(float brightness);
    void _random(float brightness);
    void _randomLeadColor(float brightness);
    void _randomLeadGradient(float brightness);
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



void CommonLEDs::_singleColor(float brightness) {
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(
      data["input_color"][0].as<int>() * brightness,
      data["input_color"][1].as<int>() * brightness,
      data["input_color"][2].as<int>() * brightness
    );
  }
}


void CommonLEDs::_gradient(float brightness) {
  float r1 = data["input_color1"][0].as<float>() / NUM_LEDS;
  float r2 = data["input_color2"][0].as<float>() / NUM_LEDS;
  float g1 = data["input_color1"][1].as<float>() / NUM_LEDS;
  float g2 = data["input_color2"][1].as<float>() / NUM_LEDS;
  float b1 = data["input_color1"][2].as<float>() / NUM_LEDS;
  float b2 = data["input_color2"][2].as<float>() / NUM_LEDS;

  float steps1[NUM_LEDS][3];
  float steps2[NUM_LEDS][3];

  for(int i=0; i<NUM_LEDS; i++) {
    steps1[i][0] = r1 * i;
    steps1[i][1] = g1 * i;
    steps1[i][2] = b1 * i;

    steps2[NUM_LEDS - 1 - i][0] = r2 * i;
    steps2[NUM_LEDS - 1 - i][1] = g2 * i;
    steps2[NUM_LEDS - 1 - i][2] = b2 * i;
  }

  for(int i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(
      (steps1[i][0] * 2 + steps2[i][0] * 2) / 2 * brightness,
      (steps1[i][1] * 2 + steps2[i][1] * 2) / 2 * brightness,
      (steps1[i][2] * 2 + steps2[i][2] * 2) / 2 * brightness
    );
  }
}


void CommonLEDs::_streamline(float brightness) {
  int offset = data["offset"].as<int>();
  int currentLength = data["current_length"].as<int>();
  for(int i=0; i<currentLength; i++) {
    leds[i + offset].setRGB(
      data["led_list"][i][0].as<float>() * brightness,
      data["led_list"][i][1].as<float>() * brightness,
      data["led_list"][i][2].as<float>() * brightness
    );
  }
}


void CommonLEDs::_random(float brightness) {

}


void CommonLEDs::_randomLeadColor(float brightness) {

}

void CommonLEDs::_randomLeadGradient(float brightness) {

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
  float brightness;

  if(data.containsKey("brightness")) {
    brightness = data["brightness"].as<float>();
    if (brightness > 1) {
      brightness = 1;
    }
  } else {
    brightness = 1;
  }

  if(mode == "single_color") {
    _singleColor(brightness);
  } else if(mode == "streamline") {
    _streamline(brightness); 
  } else if(mode == "random") {
    _random(brightness); 
  } else if(mode == "off") {
    _off(); 
  } else if(mode == "gradient") {
    _gradient(brightness); 
  } else if(mode == "randomLeadColor") {
    _randomLeadColor(brightness); 
  } else if(mode == "randomLeadGradient") {
    _randomLeadGradient(brightness); 
  }
}
