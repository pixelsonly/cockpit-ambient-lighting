# SimHub setup

How to connect [SimHub](https://www.simhubdash.com/) to the Arduino so it
streams ambient colours to the firmware over the **Adalight** protocol.

The firmware is a generic Adalight receiver, so the actual effects come from
SimHub — this rig uses the [Daniel Newman Racing](https://www.danielnewmanracing.com/)
ambient LED profile, which outputs a left/right colour pair (2 zones).

## Prerequisites

- SimHub installed on the PC running your sim
- The DNR ambient LED profile imported into SimHub (optional, but it's what this
  rig is tuned for)
- Firmware flashed to the Arduino (see the [README](../README.md))
- The Arduino connected over USB and assigned a COM port

## 1. Find the serial port

- **Windows:** Device Manager → _Ports (COM & LPT)_ → note the `COMx` number.
- **Linux/macOS:** usually `/dev/ttyACM0` or `/dev/cu.usbmodemXXXX`.

> The firmware opens the serial link at **115200 baud** (`SERIAL_RATE`). SimHub
> must be configured to match.

## 2. Configure the Adalight device

1. Open SimHub → **RGB LEDs / Devices** and add an **Adalight** device (the DNR
   ambient profile sets this up for you).
2. Select the Arduino's COM port and set the baud rate to **115200**.
3. Set the **number of LEDs** to the number of colour zones you want:
   - **2** — the DNR 2-zone ambient: zone 0 → entire **left** panel, zone 1 →
     entire **right** panel. This is the tested default.
   - **Higher** (e.g. 8, 16, 32, up to 96) — a finer gradient. The firmware
     spreads the zones evenly across the 512-pixel canvas; the first half lights
     the left panel, the second half the right. No firmware change is needed.
     (96 is the `MAX_ZONES` cap that keeps the Uno's SRAM safe.)

> ⚠️ **The firmware reinterprets the Adalight "LED count" as a zone count** and
> stretches each zone across `512 / count` physical LEDs. So you set SimHub to a
> handful of zones, *not* to 512, unless you genuinely want per-pixel control.

## 3. Adalight frame format

The firmware parses the standard Adalight serial frame in
`cockpit-ambient-lighting.ino`:

| Bytes | Field | Meaning |
| ----- | ----- | ------- |
| `41 64 61` | Magic word | ASCII `"Ada"` — marks the start of a frame |
| 1 | Count high | `count` high byte |
| 1 | Count low | `count` low byte |
| 1 | Checksum | `(countHigh ^ countLow) ^ 0x55` |
| `count × 3` | Pixel data | `R, G, B` per zone, in order |

Example frame for the 2-zone ambient (zone 0 = red, zone 1 = blue):

```
41 64 61      "Ada"
00 02         count = 2
57            checksum = (0x00 ^ 0x02) ^ 0x55
FF 00 00      zone 0 → left panel  (red)
00 00 FF      zone 1 → right panel (blue)
```

## 4. Verify

1. **At power-on**, both panels sweep red → green → blue → off. If a panel stays
   dark or shows the wrong colour, fix the wiring before going further.
2. Start a session in your sim.
3. Both panels should react to telemetry (RPM, flags, ambient colour, etc.).
4. No reaction? Check:
   - Correct COM port and **115200** baud on both sides
   - The Adalight device is enabled and pointed at the right port
   - The Arduino is powered and the panels have their own 5V supply

## Troubleshooting

| Symptom | Likely cause |
| ------- | ------------ |
| Nothing lights up | Wrong data pin, no common ground, or panels unpowered |
| Both panels show the same content | SimHub LED count is 1 (single zone) — set it to 2+ |
| Left/right panels swapped | Uncomment `SWAP_PANELS` in the sketch (or swap D5/D6) |
| Garbled / random colours | Baud mismatch, or Adalight frame ≠ parser |
| First LED wrong, rest OK | Missing series resistor / colour-order mismatch |
| Flicker under load | Undersized PSU or missing smoothing capacitor |
