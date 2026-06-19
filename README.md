<div align="center">

# cockpit-ambient-lighting

**Telemetry-driven ambient lighting for a sim racing cockpit.**

[![Compile](https://github.com/pixelsonly/cockpit-ambient-lighting/actions/workflows/compile.yml/badge.svg)](https://github.com/pixelsonly/cockpit-ambient-lighting/actions/workflows/compile.yml)
[![Lint](https://github.com/pixelsonly/cockpit-ambient-lighting/actions/workflows/lint.yml/badge.svg)](https://github.com/pixelsonly/cockpit-ambient-lighting/actions/workflows/lint.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Board: Arduino Uno R3](https://img.shields.io/badge/board-Arduino%20Uno%20R3-00979D.svg)](https://store.arduino.cc/products/arduino-uno-rev3)
[![Driven by SimHub](https://img.shields.io/badge/telemetry-SimHub-orange.svg)](https://www.simhubdash.com/)

</div>

---

## Overview

`cockpit-ambient-lighting` is the Arduino firmware behind a custom ambient
lighting rig for a sim racing cockpit. [SimHub](https://www.simhubdash.com/)
streams live game telemetry to an **Arduino Uno R3** over USB serial, and the
firmware translates that into colour and animation on an addressable LED strip
using [FastLED](https://fastled.io).

```
┌──────────┐   telemetry   ┌─────────┐   USB serial   ┌──────────────┐   data   ┌────────────┐
│   Game   │ ────────────▶ │ SimHub  │ ─────────────▶ │ Arduino Uno  │ ───────▶ │  LED strip │
│ (sim)    │               │ (PC)    │   115200 baud  │ R3 + FastLED │  WS2812B │  (WS2812B) │
└──────────┘               └─────────┘                └──────────────┘          └────────────┘
```

> **Status:** repository scaffolding is in place. The sketch
> (`cockpit-ambient-lighting.ino`) is currently a compilable placeholder —
> drop the real firmware in to replace it.

## Features

<!-- TODO: fill in once the firmware lands. Suggested bullets: -->
- RPM / shift-light style strip animation
- Flag and event colours (yellow, blue, white, etc.)
- Configurable strip length, data pin, and brightness
- Driven entirely by SimHub — no extra PC software required

## Hardware

| Part | Notes |
| --- | --- |
| Arduino Uno R3 | ATmega328P, `arduino:avr:uno` |
| Addressable LED strip | WS2812B / SK6812 (FastLED-compatible) |
| 5V power supply | Sized for your LED count (~60 mA/LED at full white) |
| USB cable | Arduino ↔ PC running SimHub |

Full wiring, power budgeting, and a recommended logic-level / capacitor setup
are in **[docs/HARDWARE.md](docs/HARDWARE.md)**.

## Quick start

### 1. Install the toolchain

Use either the Arduino IDE 2.x or the Arduino CLI.

- **Arduino IDE:** install the **FastLED** library via _Tools → Manage
  Libraries_, then open this folder.
- **Arduino CLI:** the pinned board + library versions live in
  [`sketch.yaml`](sketch.yaml), so a profile build resolves everything for you:

  ```bash
  arduino-cli compile --profile uno
  arduino-cli upload  --profile uno -p /dev/ttyACM0   # adjust the port
  ```

### 2. Configure your strip

Edit the constants at the top of `cockpit-ambient-lighting.ino` (data pin, LED
count, brightness, colour order) to match your wiring.

### 3. Flash and wire up SimHub

Upload the firmware, then point SimHub's serial output at the Arduino. Full
walkthrough: **[docs/SIMHUB_SETUP.md](docs/SIMHUB_SETUP.md)**.

## Repository layout

```
.
├── cockpit-ambient-lighting.ino   # Firmware (placeholder for now)
├── sketch.yaml                    # Arduino CLI build profile (pinned versions)
├── .clang-format / .editorconfig  # Code style
├── docs/                          # Hardware + SimHub setup guides
└── .github/                       # CI workflows, issue/PR templates
```

## Contributing

Issues and pull requests are welcome — see
[CONTRIBUTING.md](CONTRIBUTING.md) and our
[Code of Conduct](CODE_OF_CONDUCT.md). All PRs are compile-checked against the
Arduino Uno R3 in CI.

## License

[MIT](LICENSE) © Pixelsonly Racing

## Acknowledgements

- [SimHub](https://www.simhubdash.com/) — telemetry and serial output
- [FastLED](https://fastled.io) — the addressable LED library
