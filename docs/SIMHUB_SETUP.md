# SimHub setup

How to connect [SimHub](https://www.simhubdash.com/) to the Arduino so it
streams telemetry to the firmware.

## Prerequisites

- SimHub installed on the PC running your sim
- Firmware flashed to the Arduino (see the [README](../README.md))
- The Arduino connected over USB and assigned a COM port

## 1. Find the serial port

- **Windows:** Device Manager → _Ports (COM & LPT)_ → note the `COMx` number.
- **Linux/macOS:** usually `/dev/ttyACM0` or `/dev/cu.usbmodemXXXX`.

> The firmware opens the serial link at **115200 baud** (`kSerialBaud`). SimHub
> must be configured to match.

## 2. Configure SimHub serial output

1. Open SimHub → **Arduino** (or **Serial / Custom serial devices**).
2. Select the Arduino's COM port and set the baud rate to **115200**.
3. Define the message SimHub sends each update.

> ⚠️ **Keep this in sync with the firmware.** The byte/line format SimHub emits
> must exactly match what `loop()` parses in `cockpit-ambient-lighting.ino`.

## 3. Message format

<!-- TODO: document the exact telemetry frame once the firmware is finalised. -->

| Field | Type | Example | Meaning |
| ----- | ---- | ------- | ------- |
| `<TODO>` | `<TODO>` | `<TODO>` | `<TODO>` |

Example frame:

```
<TODO: paste the SimHub message template here>
```

## 4. Verify

1. Start a session in your sim.
2. The strip should react to telemetry (RPM, flags, etc.).
3. No reaction? Check:
   - Correct COM port and **115200** baud on both sides
   - The SimHub message template matches the firmware parser
   - The Arduino is powered and the strip has its own 5V supply

## Troubleshooting

| Symptom | Likely cause |
| ------- | ------------ |
| Nothing lights up | Wrong data pin, no common ground, or strip unpowered |
| Garbled / random colours | Baud mismatch, or message format ≠ parser |
| First LED wrong, rest OK | Missing series resistor / colour-order mismatch |
| Flicker under load | Undersized PSU or missing smoothing capacitor |
