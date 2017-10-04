#include "FastLED.h"

#define NUM_LEDS 10

CRGBArray<NUM_LEDS> leds;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  FastLED.addLeds<WS2811, 6, BRG>(leds, NUM_LEDS);
  leds.fill_solid(CRGB::Black);
//  fastLED
//  FastLED.setBrightness(50);
}

void loop() {
//  loopFlash();
  loopBreathe();
  FastLED.show(); 
}

void loopFlash(){
  static bool flash = true;
  leds.fill_solid(flash ? (CRGB)CHSV(160, 110, 255) : CRGB::Black);
  delay(10+random(140));
  
  flash = !flash;
}

void loopBreathe(){
  float p = t(-3000);
  float p2 = t(8000);
  
  // put your main code here, to run repeatedly:
  for(int i = 0; i < NUM_LEDS; i++) {
    float a1 = pToBrightness(p, i, NUM_LEDS, 1);
    float a2 = pToBrightness(p2, i, NUM_LEDS, 2);
    leds[i] = CHSV(160, 110, min(255, 60+25*ts(2000)+150*a1+a2*200));
  }
}

float pToBrightness(float p, float led, float leds){
  return pToBrightness(p, led, leds, 1);
}

float pToBrightness(float p, float led, float leds, float width){
  //difference between led pos and percentage
  float diff = abs( (led/leds) - (p-floor(p)) );
  
  //loop around if closer to other side
  if(0.5 < diff)
    diff = 1-diff;
  
  return max(0, 1 - diff*NUM_LEDS/width);
}

float t(int interval){
  return (float)(millis()%interval)/interval;
}

float ts(int interval){
  return sin(t(interval)*TWO_PI);
}

