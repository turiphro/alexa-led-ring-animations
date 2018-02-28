This Arduino sketch allows easy creation of Alexa LED rings.
The sketch includes all the animations currently used by the
Amazon Echo devices.

The sketch might be useful for adding the recognisable LED
interface to custom devices built with
[ALexa AVS](https://developer.amazon.com/alexa-voice-service),
or for pimping any other place in need for blinking lights.

Simply connect a NeoPixel (WS2812B) ring of 12+ RGB LEDs to
pin 2, and add custom logic controlling which animations to
show.

Dependencies:
- [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) library


![example animation](ring-animation.gif)
