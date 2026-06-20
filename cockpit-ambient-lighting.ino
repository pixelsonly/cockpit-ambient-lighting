#include "FastLED.h"

// ─── Hardware ────────────────────────────────────────────────────────────────
// Two BTF-LIGHTING WS2812B 8x32 panels (256 pixels each) on one Arduino Uno.
// Both panels show the SAME full gradient (mirrored): SimHub (Adalight) streams
// `count` colour zones and each panel spreads all of them across its own 256
// pixels. The two panels are mounted as mirror images, so the buffer is reversed
// in place for one of them to keep both gradients flowing the same way.
#define LEDS_PER_PANEL 256  // 8 x 32 per BTF-LIGHTING panel
#define LEFT_PANEL_PIN 6    // Data pin feeding one panel
#define RIGHT_PANEL_PIN 5   // Data pin feeding the other panel
#define SERIAL_RATE 115200

// One panel is mounted mirror-image, so its pixel order is reversed to match the
// other. By default the panel on RIGHT_PANEL_PIN is the reversed one; if the
// other panel is the one that looks backwards, uncomment this instead:
// #define REVERSE_LEFT_INSTEAD

// Adalight frame: 'A' 'd' 'a' + count_hi + count_lo + checksum + count*RGB.
// SimHub sends `count` colour zones; each zone is spread evenly across the 256
// pixels of every panel. count = 2 (the DNR 2-zone ambient profile) gives a
// left-half / right-half colour split; raise it for a finer gradient. Both
// panels always show identical content (one flipped to match orientation).
uint8_t prefix[] = {'A', 'd', 'a'};
uint16_t hi, lo, chk;
uint16_t groupCount;  // Number of zones provided by SimHub

// One shared buffer; each panel is shown from it in turn (the flipped one after
// the buffer is reversed in place).
CRGB leds[LEDS_PER_PANEL];
CLEDController* normalPanel;
CLEDController* flippedPanel;

void setup() {
#ifdef REVERSE_LEFT_INSTEAD
  flippedPanel = &FastLED.addLeds<NEOPIXEL, LEFT_PANEL_PIN>(leds, LEDS_PER_PANEL);
  normalPanel = &FastLED.addLeds<NEOPIXEL, RIGHT_PANEL_PIN>(leds, LEDS_PER_PANEL);
#else
  normalPanel = &FastLED.addLeds<NEOPIXEL, LEFT_PANEL_PIN>(leds, LEDS_PER_PANEL);
  flippedPanel = &FastLED.addLeds<NEOPIXEL, RIGHT_PANEL_PIN>(leds, LEDS_PER_PANEL);
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
  if (groupCount == 0 || groupCount > LEDS_PER_PANEL) {
    return;  // Need 1..256 zones to map onto a 256-pixel panel.
  }

  // Fill the buffer as the frame arrives, then show. We must not show() mid
  // frame: it disables interrupts for ~8 ms per panel and the Uno would drop the
  // serial bytes for the zones still in flight.
  uint16_t ledsPerGroup = LEDS_PER_PANEL / groupCount;
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
    if (groupIndex == groupCount - 1) {
      endIndex = LEDS_PER_PANEL;  // Last zone absorbs any remainder — no stale pixels.
    }
    for (uint16_t i = startIndex; i < endIndex; i++) {
      leds[i] = color;
    }
  }

  // Show the normally-oriented panel, then reverse the buffer in place and show
  // the mirror-mounted panel so both gradients flow the same way.
  normalPanel->showLeds(255);
  for (uint16_t i = 0; i < LEDS_PER_PANEL / 2; i++) {
    CRGB swap = leds[i];
    leds[i] = leds[LEDS_PER_PANEL - 1 - i];
    leds[LEDS_PER_PANEL - 1 - i] = swap;
  }
  flippedPanel->showLeds(255);
}
