#include "FastLED.h"

#define NUM_STRIPS 2

#define STRIP1 2*4
#define STRIP1PIN 3

#define STRIP2 2
#define STRIP2PIN 6

#define NUM_LEDS STRIP1+STRIP2

CRGB leds[NUM_LEDS];


const int accelPins[] = {A2, A1, A0};

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

  //motors
  motor1.setup();
  motor2.setup();

  //strips
  FastLED.addLeds<WS2811, STRIP1PIN, BRG>(leds, STRIP1);
//  FastLED.addLeds<WS2811, STRIP2PIN, BRG>(leds, STRIP1, STRIP2);
  FastLED.showColor(CRGB::Black);
//  FastLED.setBrightness(80);

  //accelerometer
  for (int i = 0; i < 3; i++) {
    pinMode(accelPins[i], INPUT);
  }
}

int raws[3];
int last[3];
int lows[3] = {460, 480, 480};
int highs[3] = {180, 180, 180};

float smoothedDiff[3];
bool lastIsMoving = false;

void loop() {
  float maxDiff = 0;
  for(int i = 0; i < 3; i++){
    raws[i] = analogRead(accelPins[i]);

    smoothedDiff[i] = smoothedDiff[i] + (abs(last[i] - raws[i]) - smoothedDiff[i]) / 128;
    
    if(maxDiff < smoothedDiff[i])
      maxDiff = smoothedDiff[i];

    last[i] = raws[i];
  }
  bool isMoving = 5 < maxDiff;
  
//  Serial.print(maxDiff);
//  Serial.print('\t');
//  Serial.println(isMoving ? 10 : 0);

  if(isMoving && !lastIsMoving)
    resetLightning();

  if(isMoving)
    loopLightning();
  else
    FastLED.showColor(CRGB::Black);
  //  loopFan();
  //  loopFlash();
  //  loopBreathe();

  lastIsMoving = isMoving;
}

void loopFan() {
  uint8_t speed = 255 / 2 + beatsin8(20) / 2;
  motor1.speed(speed);
  motor2.speed(speed);
}

CRGB* ledMap[NUM_LEDS];
int length;

void add(int strip, int start, int count) {
  int iterator = (0 < count) ? 1 : -1;
  for (int i = start; i != (start + count); i += iterator) {
    ledMap[length] = &FastLED[strip][i];
    length++;
  }
}

void add(int start, int count) {
  add(0, start, count);
}

void set(int ledI, CRGB color) {
  *ledMap[ledI] = color;
}

bool randomPercent(int percent){
  return random(0, 100) < percent;
}

void addLedsTesting() {
  if(randomPercent(30)){
    add(7, -2);
  }else{
    add(0, 1);
    if(randomPercent(50))
      add(5, -2);
    else
      add(1, 3);
  }
}

int start = NULL;
int fillPeriod = 1;
int holdPeriod;
int blackPeriod;

void resetLightning(){
    start = millis();
    length = 0;
    FastLED.showColor(CRGB::Black);

    fillPeriod = random(50, 500);
    holdPeriod = random(300, 600);
    blackPeriod = random(20, 500);
    
//    addLedsTesting();
    add(0, STRIP1);

//    int strikes = randomPercent(30) ? 1 : 
//    1;
//    for(int i = 0; i < strikes; i++){
//    }
//    addAllLeds();
}

void loopLightning() {
  int duration = (millis() - start);
  float progress = (float)duration / fillPeriod;

  //stage setup
  if (start == NULL || (fillPeriod + holdPeriod + blackPeriod) < duration) {
    resetLightning();
    progress = 0;
  }

  Serial.println(progress);

  //set color
  CRGB cur;
  CRGB color = CHSV(160, 60, 255);
  for (int i = 0; i < length; i++) {
    cur = color;

    float ball1 = sinball(0.5 + ts(2700) / 2, i, length, 0.2);
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

