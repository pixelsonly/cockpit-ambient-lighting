#include "FastLED.h"

// ─── Hardware ────────────────────────────────────────────────────────────────
// Two BTF-LIGHTING WS2812B 8x32 panels (256 pixels each) on one Arduino Uno.
// The panels are mounted separately (left & right of the cockpit) but treated as
// a single continuous canvas: virtual pixels [0, 256) drive the LEFT panel and
// [256, 512) drive the RIGHT panel. SimHub (Adalight) streams the colours.
//
// RAM note: the Uno has only 2 KB of SRAM. A full 512-LED buffer (1536 bytes)
// leaves too little headroom and the compiler warns of instability, so we keep
// ONE 256-LED buffer and render each panel into it in turn. The frame's zone
// colours are buffered first (see loop()) so we never show a panel mid-frame.
#define LEDS_PER_PANEL 256                // 8 x 32 per BTF-LIGHTING panel
#define CANVAS_LEDS (LEDS_PER_PANEL * 2)  // 512 virtual pixels across both panels
#define LEFT_PANEL_PIN 6                  // Data pin feeding the LEFT panel
#define RIGHT_PANEL_PIN 5                 // Data pin feeding the RIGHT panel
#define SERIAL_RATE 115200
#define MAX_ZONES 96  // Most SimHub Adalight colour zones we can buffer (SRAM)

// If the panels come up swapped (left content on the right panel), uncomment:
// #define SWAP_PANELS

// Adalight frame: 'A' 'd' 'a' + count_hi + count_lo + checksum + count*RGB.
// SimHub sends `count` colour zones; each zone is spread evenly across the 512
// virtual canvas. With SimHub's Adalight LED count = 2 (the DNR 2-zone ambient
// profile) zone 0 fills the LEFT panel and zone 1 fills the RIGHT panel. Raise
// SimHub's LED count (up to MAX_ZONES) for a finer gradient across both panels.
uint8_t prefix[] = {'A', 'd', 'a'};
uint16_t hi, lo, chk;
uint16_t groupCount;  // Number of zones provided by SimHub

// One 256-LED buffer, rendered once per panel; zones[] holds the frame's colours
// so the whole frame is read before either panel is shown (see RAM note above).
CRGB leds[LEDS_PER_PANEL];
CRGB zones[MAX_ZONES];
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

  // Read the WHOLE frame into zones[] BEFORE touching the LEDs. Showing a panel
  // mid-frame would disable interrupts for ~8 ms and drop the serial bytes for
  // the zones still in flight — which is why counts above 2 only lit the left
  // panel. Buffering first, then rendering, keeps both panels correct.
  for (uint16_t z = 0; z < groupCount; z++) {
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
    if (z < MAX_ZONES) {
      zones[z] = CRGB(r, g, b);
    }
  }

  // If SimHub streams more zones than we can buffer, skip this frame rather than
  // render a truncated gradient. Raise MAX_ZONES (watch SRAM) if you need more.
  if (groupCount > MAX_ZONES) {
    return;
  }

  // Spread the zones across the 512-pixel virtual canvas and flush each panel.
  // Both reads are done, so neither showLeds() can drop incoming serial data.
  uint16_t ledsPerGroup = CANVAS_LEDS / groupCount;

  for (uint16_t p = 0; p < LEDS_PER_PANEL; p++) {
    uint16_t zone = p / ledsPerGroup;  // Left panel = virtual pixels [0, 256).
    if (zone >= groupCount) {
      zone = groupCount - 1;  // Trailing remainder → last zone.
    }
    leds[p] = zones[zone];
  }
  leftPanel->showLeds(255);

  for (uint16_t p = 0; p < LEDS_PER_PANEL; p++) {
    uint16_t zone = (LEDS_PER_PANEL + p) / ledsPerGroup;  // Right = [256, 512).
    if (zone >= groupCount) {
      zone = groupCount - 1;
    }
    leds[p] = zones[zone];
  }
  rightPanel->showLeds(255);
}
