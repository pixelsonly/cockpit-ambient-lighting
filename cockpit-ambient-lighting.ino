#include "FastLED.h"
#define NUM_LEDS 256   // Total physical LED count
#define DATA_PIN_1 7   // First LED strip pin
#define DATA_PIN_2 6   // Second LED strip pin
#define serialRate 115200
// Adalight magic word prefix
uint8_t prefix[] = {'A', 'd', 'a'};
uint16_t hi, lo, chk;
uint16_t groupCount;  // Number of groups (as provided by SimHub)
// LED array (Shared for both strips)
CRGB leds[NUM_LEDS];
void setup() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN_1>(leds, NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, DATA_PIN_2>(leds, NUM_LEDS); // Mirrors DATA_PIN_1
    // Initial test sequence
    FastLED.showColor(CRGB(255, 0, 0)); delay(500);
    FastLED.showColor(CRGB(0, 255, 0)); delay(500);
    FastLED.showColor(CRGB(0, 0, 255)); delay(500);
    FastLED.showColor(CRGB(0, 0, 0));
    Serial.begin(serialRate);
    Serial.print("Ada\n"); // Send magic word string to host
}
void loop() {
    // Wait for the magic word "Ada"
    for (uint8_t i = 0; i < sizeof(prefix); ++i) {
        while (!Serial.available());
        if (prefix[i] != Serial.read()) {
            i = 0;  // Reset if mismatch
            continue;
        }
    }
    // Read group count bytes and checksum.
    while (!Serial.available());
    hi = Serial.read();
    while (!Serial.available());
    lo = Serial.read();
    while (!Serial.available());
    chk = Serial.read();
    // Validate checksum: expected chk = (hi ^ lo) ^ 0x55
    if (chk != ((hi ^ lo) ^ 0x55)) {
        return;  // Invalid frame, skip updating LEDs
    }
    // Interpret the header bytes as the number of groups
    groupCount = (hi << 8) | lo;
    // Calculate how many physical LEDs per group.
    uint16_t ledsPerGroup = NUM_LEDS / groupCount;
    // For each group, read one RGB triplet and apply it to the group segment.
    for (uint16_t groupIndex = 0; groupIndex < groupCount; groupIndex++) {
        byte r, g_val, b;
        while (!Serial.available());
        r = Serial.read();
        while (!Serial.available());
        g_val = Serial.read();
        while (!Serial.available());
        b = Serial.read();
        // Set all LEDs in the group segment to the received color.
        uint16_t startIndex = groupIndex * ledsPerGroup;
        uint16_t endIndex = startIndex + ledsPerGroup;
        for (uint16_t i = startIndex; i < endIndex; i++) {
            leds[i] = CRGB(r, g_val, b);
        }
    }
    FastLED.show();  // Outputs same data to both pins
}

