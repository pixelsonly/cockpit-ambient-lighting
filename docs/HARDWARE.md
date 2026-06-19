# Hardware & wiring

> Reference wiring for the cockpit-ambient-lighting rig. Adjust to match your
> own build, then update this doc so it stays the source of truth.

## Bill of materials

| # | Part | Notes |
| - | ---- | ----- |
| 1 | Arduino Uno R3 | ATmega328P |
| 1 | WS2812B / SK6812 LED strip | `<TODO: length / LED count>` |
| 1 | 5V power supply | See [power budget](#power-budget) |
| 1 | 300–500 Ω resistor | In series with the data line |
| 1 | 1000 µF capacitor | Across strip 5V / GND, near the strip |
| 1 | USB cable | Arduino ↔ PC |

## Wiring

```
Arduino Uno R3                     WS2812B strip
-------------                      -------------
D6  ──[ 330 Ω ]────────────────▶  DIN
5V  ───────────────┬───────────▶  +5V
GND ──────────────┬┴───────────▶  GND
                  │ │
              1000 µF (+ to 5V, − to GND)
```

- **Data pin:** `D6` by default (`kDataPin` in the sketch). Any digital pin works — keep it short.
- **Common ground:** the Arduino GND, the strip GND, and the PSU GND must all be tied together.
- **Series resistor:** 300–500 Ω on the data line protects the first LED.
- **Capacitor:** 1000 µF across the strip's 5V/GND smooths inrush current.

> **Powering the strip:** more than a few LEDs at full brightness will exceed
> what the Arduino's 5V pin can safely supply — power the strip from a dedicated
> 5V supply and only share **ground** with the Arduino.

## Power budget

WS2812B LEDs draw up to ~60 mA each at full white. Size the supply with margin:

| LED count | Worst-case current (full white) | Suggested PSU |
| --------- | ------------------------------- | ------------- |
| 30  | ~1.8 A | 5V 3A |
| 60  | ~3.6 A | 5V 5A |
| 144 | ~8.6 A | 5V 10A |

In practice, capping brightness (`FastLED.setBrightness`) or using
`FastLED.setMaxPowerInVoltsAndMilliamps()` keeps draw well below worst case.

## Photos / diagram

<!-- TODO: add a wiring photo or Fritzing/diagram export here. -->
