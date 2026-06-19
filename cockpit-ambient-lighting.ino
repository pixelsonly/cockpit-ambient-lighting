#include "FastLED.h"

// ─── Hardware ────────────────────────────────────────────────────────────────
// Two BTF-LIGHTING WS2812B 8x32 panels (256 pixels each) on one Arduino Uno.
// The panels are mounted separately (left & right of the cockpit) but treated as
// a single continuous canvas: virtual pixels [0, 256) drive the LEFT panel and
// [256, 512) drive the RIGHT panel. SimHub (Adalight) streams the colours.
//
// RAM note: the Uno has only 2 KB of SRAM. A full 512-LED buffer (1536 bytes)
// leaves too little headroom and the compiler warns of instability, so instead
// we keep ONE 256-LED buffer and flush it twice per frame — once per panel.
#define LEDS_PER_PANEL 256                // 8 x 32 per BTF-LIGHTING panel
#define CANVAS_LEDS (LEDS_PER_PANEL * 2)  // 512 virtual pixels across both panels
#define LEFT_PANEL_PIN 7                  // Data pin feeding the LEFT panel
#define RIGHT_PANEL_PIN 6                 // Data pin feeding the RIGHT panel
#define SERIAL_RATE 115200

// If the panels come up swapped (left content on the right panel), uncomment:
// #define SWAP_PANELS

// Adalight frame: 'A' 'd' 'a' + count_hi + count_lo + checksum + count*RGB.
// SimHub sends `count` colour zones; each zone is spread evenly across the 512
// virtual canvas. With SimHub's Adalight LED count = 2 (the DNR 2-zone ambient
// profile) zone 0 fills the LEFT panel and zone 1 fills the RIGHT panel. Raise
// SimHub's LED count later and the same logic renders a finer gradient across
// both panels — no firmware change needed.
uint8_t prefix[] = {'A', 'd', 'a'};
uint16_t hi, lo, chk;
uint16_t groupCount;  // Number of zones provided by SimHub

// One 256-LED buffer, reused for each panel in turn (see RAM note above).
CRGB leds[LEDS_PER_PANEL];
CLEDController* leftPanel;
CLEDController* rightPanel;

void setup() {
#ifdef SWAP_PANELS
  leftPanel = &FastLED.addLeds<NEOPIXEL, RIGHT_PANEL_PIN>(leds, LEDS_PER_PANEL);
  rightPanel = &FastLED.addLeds<NEOPIXEL, LEFT_PANEL_PIN>(leds, LEDS_PER_PANEL);
#else
  leftPanel = &FastLED.addLeds<NEOPIXEL, LEFT_PANEL_PIN>(leds, LEDS_PER_PANEL);
  rightPanel = &FastLED.addLeds<NEOPIXEL, RIGHT_PANEL_PIN>(leds, LEDS_PER_PANEL);
#endif

  // Power-on test sweep — also confirms both panels are wired correctly.
  FastLED.showColor(CRGB(255, 0, 0));
  delay(500);
  FastLED.showColor(CRGB(0, 255, 0));
  delay(500);
  FastLED.showColor(CRGB(0, 0, 255));
  delay(500);
  FastLED.showColor(CRGB(0, 0, 0));

  Serial.begin(SERIAL_RATE);
  Serial.print("Ada\n");  // Announce readiness to the host (SimHub).
}

void loop() {
  // Wait for the magic word "Ada".
  for (uint8_t i = 0; i < sizeof(prefix); ++i) {
    while (!Serial.available()) {
    }
    if (prefix[i] != Serial.read()) {
      i = 0;  // Reset on mismatch.
      continue;
    }
  }

  // Read the zone count (high byte first) and the checksum.
  while (!Serial.available()) {
  }
  hi = Serial.read();
  while (!Serial.available()) {
  }
  lo = Serial.read();
  while (!Serial.available()) {
  }
  chk = Serial.read();

  // Validate: expected checksum = (hi ^ lo) ^ 0x55.
  if (chk != ((hi ^ lo) ^ 0x55)) {
    return;  // Corrupt header — skip this frame.
  }

  groupCount = (hi << 8) | lo;
  if (groupCount == 0) {
    return;  // Guard against a divide-by-zero on a malformed header.
  }

  // Spread each zone's colour across the 512-pixel virtual canvas. Virtual
  // indices march upward, so every left pixel (< 256) is written before any
  // right pixel; we flush the left panel the moment we cross the boundary, then
  // reuse the same buffer for the right panel.
  uint16_t ledsPerGroup = CANVAS_LEDS / groupCount;
  bool leftFlushed = false;
  for (uint16_t groupIndex = 0; groupIndex < groupCount; groupIndex++) {
    byte r, g, b;
    while (!Serial.available()) {
    }
    r = Serial.read();
    while (!Serial.available()) {
    }
    g = Serial.read();
    while (!Serial.available()) {
    }
    b = Serial.read();
    CRGB color = CRGB(r, g, b);

    uint16_t startIndex = groupIndex * ledsPerGroup;
    uint16_t endIndex = startIndex + ledsPerGroup;
    for (uint16_t v = startIndex; v < endIndex; v++) {
      if (v < LEDS_PER_PANEL) {
        leds[v] = color;  // Left panel buffer.
      } else {
        if (!leftFlushed) {
          leftPanel->showLeds(255);  // Push the left panel before reusing the buffer.
          leftFlushed = true;
        }
        leds[v - LEDS_PER_PANEL] = color;  // Right panel buffer (reused).
      }
    }
  }

  if (!leftFlushed) {
    leftPanel->showLeds(255);  // Frame never reached the right panel (e.g. 1 zone).
  }
  rightPanel->showLeds(255);  // Push the right panel.
}
