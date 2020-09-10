# ESPFX
ESPFX is a little program, written in C++/Arduino using PlatformIO for the ESP8266 and ESP32, to light up LEDs according to UDP requests.
While it is more or less tailored to my personal setup and demand, it may be of use for others as well. Keep in mind though, that I mainly develop in Python and JavaScript, so I may have left out paradigms and best practices.
to interface this program, send a UDP requst with json as content, using the following info

**mode:**
this key defines the way, the LEDs will be addressed and may require one or more extra arguments

 - *single_color* displays a plain color on all LEDs, requires *input_color* as extra argument
 - *random_lead_color* displays a selected color but variates it a bit per LED. Also requires *input_color* as extra argument.
 - *gradient* makes one color flow into another across all LEDs, requires *input_color1* and *input_color2* as extra arguments.
- *random_lead_gradient* makes one color flow into another across all LEDs, but variates each LED color value per LED a bit. Also requires *input_color1* and *input_color2* as extra arguments.
- *random* Assigns random RGB values to each LED. Requires no extra arguments
- *streamline* takes a list of RGB values and displays them in order. The argument is named *led_list*.

**kelvin:**
This key allows to set the color temperature. ranges from 1000 to 12000 in steps of 100.
(not implemented yet)

**brightness:**
This key sets the LED brightness. It's a float ranging from 0.0 to 1.0
Its optional and defaults to 1.0

**offset and current_length:**
Those 2 keys can be used when the *mode* key is set to *streamline*.
Basically they're helpers to put the the request data onto the correct LEDs, due to split requests.
(splitting requests is necessary at about 60 LEDs for streamline because otherwise the request data becomes too big.)

---
example JSON may look like this

    {
      "mode": "single_color",
	  "input_color": [64, 255, 12],
	  "brightness": 0.75,
	  "kelvin": 3600
    }
    
or

    {
      "mode": "random_lead_gradient",
	  "input_color1": [64, 255, 12],
	  "input_color2": [128, 32, 164],
	  "brightness": 0.9,
	  "kelvin": 3800
    }
    
or

    {
      "mode": "streamline",
	  "led_list": [[64, 255, 12], [255, 0, 0], [128, 128, 255]],
	  "brightness": 0.33,
	  "kelvin": 3600
    }
