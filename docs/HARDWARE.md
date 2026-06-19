# Hardware & wiring

> Reference wiring for the cockpit-ambient-lighting rig. Adjust to match your
> own build, then update this doc so it stays the source of truth.

## Bill of materials

| # | Part | Notes |
| - | ---- | ----- |
| 1 | Arduino Uno R3 | ATmega328P |
| 2 | BTF-LIGHTING WS2812B 8×32 panel | 256 pixels each — **512 LEDs total** |
| 1 | 5V power supply | See [power budget](#power-budget) — size for 512 LEDs |
| 2 | 300–500 Ω resistor | One in series with each panel's data line |
| 2 | 1000 µF capacitor | One across each panel's 5V / GND, near the panel |
| 1 | USB cable | Arduino ↔ PC |

## Wiring

Each panel is driven from its own data pin. `D7` feeds the **left** panel and
`D6` feeds the **right** panel (swap with `SWAP_PANELS` in the sketch if needed).

```
Arduino Uno R3                         WS2812B panels
-------------                          --------------
D7  ──[ 330 Ω ]──────────────────────▶ LEFT  panel DIN
D6  ──[ 330 Ω ]──────────────────────▶ RIGHT panel DIN
GND ─────────────────────────────────▶ both panel GNDs ── PSU GND (common)

         5V PSU (+) ─────────┬────────▶ LEFT  panel +5V
                             ├────────▶ RIGHT panel +5V
                       1000 µF per panel (+ to 5V, − to GND, near each panel)
```

- **Data pins:** `LEFT_PANEL_PIN` (`D7`) and `RIGHT_PANEL_PIN` (`D6`) in the
  sketch. Any digital pins work — keep the data runs short.
- **Common ground:** the Arduino GND, both panel GNDs, and the PSU GND must all
  be tied together.
- **Series resistor:** 300–500 Ω on each data line protects the first LED.
- **Capacitor:** 1000 µF across each panel's 5V/GND smooths inrush current.
- **Do not** back-feed the panels from the Arduino's 5V pin — see the power
  budget below.

## Power budget

WS2812B LEDs draw up to ~60 mA each at full white. With **512 LEDs** that is a
serious supply, well beyond USB or the Arduino's 5V pin — use a dedicated 5V PSU
and share only **ground** with the Arduino.

| LED count | Worst-case current (full white) | Suggested PSU |
| --------- | ------------------------------- | ------------- |
| 256 (one panel)  | ~15 A | 5V 20A |
| 512 (both panels) | ~30 A | 5V 40A (or 2× 5V 20A) |

In practice a 2-zone ambient profile is nowhere near full white, so a smaller
supply often suffices — but size for the worst case if you ever run bright,
full-canvas content. Capping brightness (`FastLED.setBrightness`) or
`FastLED.setMaxPowerInVoltsAndMilliamps()` bounds the draw, **but note** that
power-limiting dims the output, which changes how the SimHub/DNR effect looks —
add it deliberately, not by default.

## Microcontroller headroom

Driving 512 individually-addressable LEDs is near the Uno's limit. The firmware
keeps a single 256-LED buffer (768 bytes) and flushes it once per panel, which
leaves comfortable SRAM headroom (~58% used). A full 512-LED buffer would leave
the ATmega328P's 2 KB of SRAM dangerously low and risk mid-session instability.
If you later want a larger per-pixel canvas, move to a board with more SRAM
(e.g. Arduino Mega 2560, Nano Every, or an ESP32) and update `sketch.yaml`.

## Photos / diagram

<!-- TODO: add a wiring photo or Fritzing/diagram export here. -->
