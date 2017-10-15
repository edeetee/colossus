#include "FastLED.h"

#define STRIP1 2*2
#define PIN1 5

#define STRIP2 2*5
#define PIN2 6

#define NUM_LEDS STRIP1+STRIP2

CRGB leds[NUM_LEDS];

struct Motor {
  int E;
  int F;
  int R;
  void setup();
  void speed(int);
};

void Motor::setup() {
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(R, OUTPUT);
  speed(0);
}

/**
   @param speed speed of the motor. Positive and negative will swap the motor direction.
*/
void Motor::speed(int speed) {
  if (speed < 0) {
    digitalWrite(R, HIGH);
    digitalWrite(F, LOW);
  } else {
    digitalWrite(F, HIGH);
    digitalWrite(R, LOW);
  }

  analogWrite(E, abs(speed));
}

Motor motor1 = {11, 10, 9};
Motor motor2 = {6, 8, 7};

void setup() {
  Serial.begin(9600);

  motor1.setup();
  motor2.setup();

  // put your setup code here, to run once:
  FastLED.addLeds<WS2811, 5, BRG>(leds, STRIP1);
  FastLED.addLeds<WS2811, 6, BRG>(leds, STRIP1, STRIP2);
  fill(CRGB::Black);
  //  FastLED.setBrightness(50);
}

void loop() {
  //  loopLightning();
  loopFan();
  //  loopFlash();
  //  loopBreathe();
}

void loopFan() {
  uint8_t speed = 255/2 + beatsin8(20) / 2;
  Serial.println(speed);
  motor1.speed(speed);
  motor2.speed(speed);
}

int ledMap[NUM_LEDS];
int length;

void add(int start, int count) {
  int iterator = (0 < count) ? 1 : -1;
  for (int i = start; i != (start + count); i += iterator) {
    ledMap[length] = i;
    length++;
  }
}

void set(int ledI, CRGB color) {
  leds[ledMap[ledI]] = color;
}

int start = NULL;
int fillPeriod = 150;
int holdPeriod = 500;
int blackPeriod = 400;

void loopLightning() {
  int duration = (millis() - start);
  float progress = (float)duration / fillPeriod;

  //stage setup
  if (start == NULL || (fillPeriod + holdPeriod + blackPeriod) < duration) {
    start = millis();
    progress = 0;
    length = 0;
    fill(CRGB::Black);

    fillPeriod = random(100, 400);
    holdPeriod = random(300, 1000);
    blackPeriod = random(100, 3000);

    if (random(2)) {
      add(0, 2);

      int r = random(4);
      if (r < 1.0)
        add(2, 4);
      else if (r < 2.0)
        add(STRIP1 + 6, 2);
      else if (r < 3.0)
        add(STRIP1 + 9, -2);
      else
        add(STRIP1 + 5, -2);

    } else {
      add(STRIP1, 4);
    }
  }

  //set color
  CRGB cur;
  CRGB color = CHSV(160, 80, 255);
  for (int i = 0; i < length; i++) {
    cur = color;

    float ball1 = sinball(0.5 + ts(-700) / 2, i, length, 0.2);
    float ball2 = sinball(0.5 + ts(200) / 2, i, length, 0.5);

    cur.nscale8(150 * ball1 + 105 * ball2);

    float show = upto(progress, i, length);

    //blackness after fill and hold
    if ((fillPeriod + holdPeriod) < duration) {
      show = 0;
    }

    cur.nscale8(255 * show);

    set(i, cur);
  }

  //apply
  FastLED.show();
}

bool isLed(int i) {
  return 0 <= i && i < NUM_LEDS;
}

// int randomness = 20;
// void doLightning(int start, int count, int interval){
//   int iteration = 0 < count ? 1 : -1;
//   for(int i = start; i != start+count; i += iteration){
//     leds[i] = CRGB::White;
//     FastLED.show();
//     delay(random(interval-randomness, interval+randomness));
//     leds[i] = blend(CRGB::White, CRGB::Black, 150);
//     currentLeds.add(i);
//     FastLED.show();
//   }
// }


// void loopFlash(){
//   static bool flash = true;
//   fill(flash ? (CRGB)CHSV(160, 110, 255) : CRGB::Black);
//   delay(10+random(140));

//   flash = !flash;
// }

// void loopBreathe(){
//   float p = t(-3000);
//   float p2 = t(8000);

//   // put your main code here, to run repeatedly:
//   for(int i = 0; i < NUM_LEDS; i++) {
//     float a1 = sinball(p, i, NUM_LEDS, 1);
//     float a2 = sinball(p2, i, NUM_LEDS, 2);
//     leds[i] = CHSV(160, 110, min(255, 60+25*ts(2000)+150*a1+a2*200));
//   }
// }

void fill(CRGB color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();
}

float upto(float p, float i, float length) {
  return max(min(( p - (i / length)) * length, 1.0), 0.0);
}

float sinball(float p, float i, float length, float radius) {
  //difference between led pos and percentage
  float diff = abs( (i / length) - (p - floor(p)) );

  //loop around if closer to other side
  if (0.5 < diff)
    diff = 1 - diff;

  if (radius < diff)
    return 0;

  return cos(diff / radius * PI / 2);
}

float t(int interval) {
  return (float)(millis() % interval) / interval;
}

float ts(int interval) {
  return sin(t(interval) * TWO_PI);
}

