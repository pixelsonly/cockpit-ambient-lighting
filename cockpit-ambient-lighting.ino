// =============================================================================
//  cockpit-ambient-lighting
//  Ambient lighting for a sim racing cockpit, driven by SimHub telemetry.
//
//  >>> PLACEHOLDER SCAFFOLD <<<
//  This is a minimal, compilable stub so CI is green from day one. Replace the
//  body with your real firmware. The pin / strip constants below are guesses —
//  set them to match your wiring (see docs/HARDWARE.md).
//
//  Target board : Arduino Uno R3 (ATmega328P)
//  LED driver   : FastLED  (https://fastled.io)
//  Data source  : SimHub "Serial / Arduino" output over USB (see docs/SIMHUB_SETUP.md)
// =============================================================================

#include <FastLED.h>

// ---- Strip configuration ----------------------------------------------------
constexpr uint8_t kDataPin = 6;          // Digital pin wired to the strip's DIN
constexpr uint16_t kNumLeds = 60;        // Number of LEDs on the strip
constexpr uint8_t kDefaultBrightness = 64;  // 0–255
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

// ---- Serial link to SimHub --------------------------------------------------
constexpr uint32_t kSerialBaud = 115200;

CRGB leds[kNumLeds];

void setup() {
  Serial.begin(kSerialBaud);

  FastLED.addLeds<LED_TYPE, kDataPin, COLOR_ORDER>(leds, kNumLeds);
  FastLED.setBrightness(kDefaultBrightness);
  FastLED.clear(true);  // Start dark
}

void loop() {
  // TODO: parse the telemetry frame SimHub sends over Serial and update `leds`,
  // then call FastLED.show(). See docs/SIMHUB_SETUP.md for the message format.
}
