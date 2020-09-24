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
    void _singleColor();
    void _gradient();
    void _streamline();
    void _random();
    void _randomLeadColor();
    void _randomLeadGradient();
    void _off(void);
    float _colorCorrection(int color, int index);
    //dirty but simple I guess
    void _generateGradient(void);
    float _generatedGradient[NUM_LEDS][3];
    float _brightness;
};

#endif


// AFAIK this bottom part should get its own file, named common.cpp
// but I'm just a stupid python dev new to this whole C++ thing.
// This should do for now :3


StaticJsonDocument<16384> data;
CRGB leds[NUM_LEDS];


float CommonLEDs::_colorCorrection(int color, int index) {
  float output;
  // the functions using pow and sqrt take about 20ms per full draw cycle
  // this is too much and the simplified multiplication should do
  // also it it optimized towards my WS2811 LEDs and gives a
  // slight reddish/warm tint on low brightness, no big deal though
  switch(index) {
    case 0:
      // red
      output = color * _brightness;
      break;
    case 1:
      // green
      // (pow(sqrt(color), 1.825) + (color / 100))
      output = color * .55 * _brightness;
      break;
    case 2:
      // blue
      // (pow(sqrt(color), 1.775) + (color / 100))
      output = color * .45 * _brightness;
      break;
    default:
      output = 0;
  }

  return output;
}


float randomLead(float input) {
  return (input * 1.5 + random(0, 127)) / 2;
}


void CommonLEDs::_generateGradient() {
  float r1 = data["input_color1"][0].as<float>() / NUM_LEDS;
  float r2 = data["input_color2"][0].as<float>() / NUM_LEDS;
  float g1 = data["input_color1"][1].as<float>() / NUM_LEDS;
  float g2 = data["input_color2"][1].as<float>() / NUM_LEDS;
  float b1 = data["input_color1"][2].as<float>() / NUM_LEDS;
  float b2 = data["input_color2"][2].as<float>() / NUM_LEDS;

  for(int i=0; i<NUM_LEDS; i++) {
    _generatedGradient[i][0] = ((r2 * i * 2) + (r1 * (NUM_LEDS - 1 - i) * 2)) / 2;
    _generatedGradient[i][1] = ((g2 * i * 2) + (g1 * (NUM_LEDS - 1 - i) * 2)) / 2;
    _generatedGradient[i][2] = ((b2 * i * 2) + (b1 * (NUM_LEDS - 1 - i) * 2)) / 2;
  }
}


CommonLEDs::CommonLEDs() {
  FastLED.addLeds<LED_CHIPSET, LEDPIN, LED_COLOR_ORDER>(leds, NUM_LEDS);
}



void CommonLEDs::_singleColor() {
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(
      _colorCorrection(data["input_color"][0].as<int>(), 0),
      _colorCorrection(data["input_color"][1].as<int>(), 1),
      _colorCorrection(data["input_color"][2].as<int>(), 2)
    );
  }
}


void CommonLEDs::_gradient() {
  _generateGradient();

  for(int i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(
      _colorCorrection(_generatedGradient[i][0], 0),
      _colorCorrection(_generatedGradient[i][1], 1),
      _colorCorrection(_generatedGradient[i][2], 2)
    );
  }
}


void CommonLEDs::_streamline() {
  int offset = data["offset"].as<int>();
  int currentLength = data["current_length"].as<int>();
  for(int i=0; i<currentLength; i++) {  
    leds[i + offset].setRGB(
      _colorCorrection(data["led_list"][i][0].as<int>(), 0),
      _colorCorrection(data["led_list"][i][1].as<int>(), 1),
      _colorCorrection(data["led_list"][i][2].as<int>(), 2)
    );
  }
}


void CommonLEDs::_random() {
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(
      _colorCorrection(random(32, 255), 0),
      _colorCorrection(random(32, 255), 1),
      _colorCorrection(random(32, 255), 2)
    );
  }
}

void CommonLEDs::_randomLeadColor() {
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(
      _colorCorrection(randomLead(data["input_color"][0].as<int>()), 0),
      _colorCorrection(randomLead(data["input_color"][1].as<int>()), 1),
      _colorCorrection(randomLead(data["input_color"][2].as<int>()), 2)
    );
  }
}

void CommonLEDs::_randomLeadGradient() {
  _generateGradient();

  for(int i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(
      _colorCorrection(randomLead(_generatedGradient[i][0]), 0),
      _colorCorrection(randomLead(_generatedGradient[i][1]), 1),
      _colorCorrection(randomLead(_generatedGradient[i][2]), 2)
    );
  }
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

  if(data.containsKey("brightness")) {
    _brightness = data["brightness"].as<float>();
    if (_brightness > 1) {
      _brightness = 1;
    }
  } else {
    _brightness = 1;
  }

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
  } else if(mode == "random_lead_color") {
    _randomLeadColor(); 
  } else if(mode == "random_lead_gradient") {
    _randomLeadGradient(); 
  }
}
