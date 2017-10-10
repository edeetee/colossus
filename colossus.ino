#include "FastLED.h"

#define NUM_LEDS 2*7

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  FastLED.addLeds<WS2811, 6, BRG>(leds, NUM_LEDS);
  fill(CRGB::Black);
//  FastLED.setBrightness(50);
}

void loop() {
  loopLightning();
//  loopFlash();
//  loopBreathe();
  FastLED.show(); 
}


int ledMap[NUM_LEDS];
int length;

void add(int start, int end){
  int iterator = (start < end) ? 1 : -1;
  for(int i = start; i != end; i += iterator){
    ledMap[length] = i;
    length++;
  }
}

void set(int ledI, CRGB color){
  leds[ledMap[ledI]] = color;
}

int start = NULL;
void loopLightning(){
  float progress = (float)(millis()-start)/500;
  if(start == NULL || 2.5 < progress){
    start = millis();
    progress = 0;
    length = 0;
    fill(CRGB::Black);

    add(0, 2);
    if(random(2)){
      add(2, 6);
    } else{
      add(9, 5);
    }
  }

  CRGB cur;
  CRGB color = CHSV(160, 80, 255);
  for(int i = 0; i < length; i++){
    cur = color;

    float ball1 = sinball(-300, i, length, 0.2);

    float ball2 = sinball(1000, i, length, 0.5);
    cur.nscale8(50*ball2 + 200*ball1 + 50);

    float show = upto(progress, i, length);
    cur.nscale8(255 * show);

    set(i, cur);
  }
}

bool isLed(int i){
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

void fill(CRGB color){
  for(int i = 0; i<NUM_LEDS; i++){
    leds[i] = color;
  }
  FastLED.show();
}

float upto(float p, float i, float length){
  return max(min(( p-floor(p) - (i/length))*length, 1.0), 0.0);
}

float sinball(float interval, float i, float length, float radius){
  //difference between led pos and percentage
  float p = t(interval);
  float diff = abs( (i/length) - (p-floor(p)) );
  
  //loop around if closer to other side
  if(0.5 < diff)
    diff = 1-diff;

  if(radius < diff)
    return 0;
  
  return cos(diff/radius*PI/2);
}

float t(int interval){
  return (float)(millis()%interval)/interval;
}

float ts(int interval){
  return sin(t(interval)*TWO_PI);
}
