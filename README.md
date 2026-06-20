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
(driven by the [Daniel Newman Racing](https://www.danielnewmanracing.com/)
ambient LED profile) streams live game colours to an **Arduino Uno R3** over USB
serial using the **Adalight** protocol, and the firmware paints them onto two
WS2812B LED panels using [FastLED](https://fastled.io).

```
┌──────────┐  telemetry  ┌─────────┐  USB serial  ┌──────────────┐  data  ┌──────────────┐
│   Game   │ ──────────▶ │ SimHub  │ ───────────▶ │ Arduino Uno  │ ─────▶ │ LEFT  panel  │
│ (sim)    │             │  + DNR  │  Adalight    │ R3 + FastLED │  D6    │ (WS2812B 8×32)│
└──────────┘             │ profile │  115200 baud │              │ ─────▶ │ RIGHT panel  │
                         └─────────┘              └──────────────┘  D5    │ (WS2812B 8×32)│
                                                                          └──────────────┘
```

Both panels show the **same** full gradient: SimHub's colour zones are spread
across each panel's 256 pixels, and both data pins are clocked from one shared
buffer, so the left and right panels mirror each other.

## Features

- **Adalight receiver** — speaks SimHub's standard Adalight serial protocol, so
  all of the effect logic (RPM, flags, ambient colour, etc.) lives in SimHub /
  the Daniel Newman Racing profile; no custom PC software required.
- **Dual mirrored panels** — both WS2812B 8×32 panels are driven from separate
  data pins (`D6`, `D5`) but show identical content from one shared buffer.
- **Configurable zone count** — SimHub can stream anywhere from 2 colour zones
  (the DNR 2-zone ambient profile) up to 256; each zone is spread evenly across
  every panel's 256 pixels, so finer SimHub layouts "just work" with no firmware
  change. The whole frame is buffered before the panels are shown, so higher
  counts render correctly.
- **RAM-safe on the Uno** — both panels share a single 256-LED buffer, keeping
  well within the ATmega328P's 2 KB of SRAM.
- **Power-on self-test** — a red/green/blue sweep at boot confirms both panels
  are wired and addressed correctly.

## Hardware

| Part | Notes |
| --- | --- |
| Arduino Uno R3 | ATmega328P, `arduino:avr:uno` |
| 2× WS2812B 8×32 panel | BTF-LIGHTING 256-pixel matrices — 512 LEDs total |
| 5V power supply | **Sized for 512 LEDs** — see [power budget](docs/HARDWARE.md#power-budget) |
| USB cable | Arduino ↔ PC running SimHub |

> ⚠️ 512 WS2812B LEDs can draw **~30 A at 5 V** at full white — far more than USB
> or the Arduino's 5V pin can supply. Power the panels from a dedicated 5V PSU
> and share only **ground** with the Arduino. Full numbers in
> [docs/HARDWARE.md](docs/HARDWARE.md).

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

### 2. Configure your panels

Edit the constants at the top of `cockpit-ambient-lighting.ino` to match your
wiring:

| Constant | Default | Meaning |
| --- | --- | --- |
| `LEDS_PER_PANEL` | `256` | Pixels per panel (8 × 32) |
| `LEFT_PANEL_PIN` | `6` | Data pin for one panel |
| `RIGHT_PANEL_PIN` | `5` | Data pin for the other panel |

### 3. Flash and wire up SimHub

Upload the firmware, then point SimHub's serial output at the Arduino. Full
walkthrough: **[docs/SIMHUB_SETUP.md](docs/SIMHUB_SETUP.md)**.

## Repository layout

```
.
├── cockpit-ambient-lighting.ino   # Firmware (dual-panel Adalight receiver)
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
- [Daniel Newman Racing](https://www.danielnewmanracing.com/) — the ambient LED profile
- [FastLED](https://fastled.io) — the addressable LED library
