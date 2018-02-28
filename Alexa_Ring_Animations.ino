/*
 * NeoPixel ring simulating the Alexa LED ring
 * Created: 2018-02-24
 * Author:  Martijn van der Veen (turiphro)
 * License: MIT License
 */

#include <Adafruit_NeoPixel.h>
    
#define DATA_PIN 2
#define DOOR_SENSOR 3
#define SIZE 16 /* Echo (Dot) has 12 LEDs, but code adapts to higher numbers */

Adafruit_NeoPixel ring = Adafruit_NeoPixel(24, DATA_PIN, NEO_GRB + NEO_KHZ800);

const int DURATION = 1000; // animation duration (ms)
const int STEP_SIZE = DURATION/SIZE;
const uint32_t NOTHING = ring.Color(255, 0, 0);
const uint32_t MUTE = ring.Color(153, 0, 0);
const uint32_t BOOTING_BG = ring.Color(0, 0, 255);
const uint32_t BOOTING_FG = ring.Color(0, 255, 255);
const uint32_t SPEAK_MIN = ring.Color(0, 0, 255);
const uint32_t SPEAK_MAX = ring.Color(0, 255, 255);
const uint32_t ERR_MIN = ring.Color(17, 17, 0);
const uint32_t ERR_MAX = ring.Color(255, 17, 0);
const uint32_t MSG_MIN = ring.Color(17, 17, 0);   // unverified
const uint32_t MSG_MAX = ring.Color(255, 255, 0); // unverified
const uint32_t CALL_MIN = ring.Color(0, 17, 0);   // unverified
const uint32_t CALL_MAX = ring.Color(0, 255, 0);  // unverified
// TODO: purple (do not disturb), orange spin (connecting)


uint32_t interpolate(uint32_t c1, uint32_t c2, double t) {
  // assumes 0.0 <= t <= 1.0
  uint8_t r = (c1>>16 & 0xFF)*(1-t) + (c2>>16 & 0xFF)*t;
  uint8_t g = (c1>>8  & 0xFF)*(1-t) + (c2>>8  & 0xFF)*t;
  uint8_t b = (c1     & 0xFF)*(1-t) + (c2     & 0xFF)*t;
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}


// for: nothing/listening, muted, do not disturb
void fill(uint32_t colour) {
  for (int i=0; i<SIZE; i++) {
    ring.setPixelColor(i, colour);
  }
}

// for: booting
void spinner(uint32_t colour_bg, uint32_t colour_fg, int pos, int width=ceil(SIZE/12.0));
void spinner(uint32_t colour_bg, uint32_t colour_fg, int pos, int width) {
  fill(colour_bg);
  for (int i=pos; i<pos+width; i++) {
    ring.setPixelColor(i % SIZE, colour_fg); // rotate clockwise
  }
}

// for: speaking, error, don't disturb, incoming call, ..
void pulse(uint32_t colour_min, uint32_t colour_max, double ratio, bool up) {
  double t = (up) ? ratio : 1 - ratio;
  uint32_t colour = interpolate(colour_min, colour_max, t);
  fill(colour);
}


/*** SKETCH ***/

int counter = 0;  // step function (spinner)
double ratio = 0; // continuous (pulse)
bool up = true;   // direction (pulse)
unsigned long last_increment = millis();
enum states {
  BLANK,
  MUTED,
  BOOTING,
  SPEAKING,
  ERROR,
  MESSAGE,
  CALL,
} state;

void setup() {
  ring.begin();
  ring.show();
  ring.setBrightness(100);
  pinMode(DOOR_SENSOR, INPUT_PULLUP);

  state = BOOTING;
}

void loop() {
  switch (state) {
    case MUTED:    fill(MUTE);                               break;
    case BOOTING:  spinner(BOOTING_BG, BOOTING_FG, counter); break;
    case SPEAKING: pulse(SPEAK_MIN, SPEAK_MAX, ratio, up);   break;
    case ERROR:    pulse(ERR_MIN, ERR_MAX, ratio, up);       break;
    case MESSAGE:  pulse(MSG_MIN, MSG_MAX, ratio, up);       break;
    case CALL:     pulse(CALL_MIN, CALL_MAX, ratio, up);     break;
    case BLANK:
    default:       fill(NOTHING);                            break;
  }
  ring.show();

  if (millis() - last_increment > STEP_SIZE || millis() < last_increment) {
    // 0->SIZE per DURATION
    counter = ++counter % SIZE;
    if (counter == 0) {
      up = !up; // swap
    }
    last_increment = millis();
  }
  // 0.0->1.0 per duration
  ratio = min(1.0, (double)(counter*STEP_SIZE + (millis() - last_increment)) / DURATION);


  // YOUR LOGIC (state transitions)
  // This example sketch shows the 'booting' animation,
  // and switches to the 'muted' animation as long as the door is closed
  int door_opened = digitalRead(DOOR_SENSOR);
  switch (state) {
    case BOOTING: if (!door_opened) { state = MUTED;   }  break;
    case MUTED:   if (door_opened)  { state = BOOTING; }  break;
    default:      state = ERROR;                          break;
  }
  
  delay(1);
}

