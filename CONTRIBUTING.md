# Contributing

Thanks for your interest in improving **cockpit-ambient-lighting**! This is a
hobby/open-source project, so contributions of all sizes are welcome — bug
reports, new lighting effects, docs, and wiring tips alike.

By participating you agree to abide by our [Code of Conduct](CODE_OF_CONDUCT.md).

## Getting set up

You can build with either the Arduino IDE 2.x or the Arduino CLI. The CLI is
what CI uses and gives you reproducible, version-pinned builds:

```bash
# Install once: https://arduino.github.io/arduino-cli/latest/installation/
arduino-cli compile --profile uno          # build (versions pinned in sketch.yaml)
arduino-cli upload  --profile uno -p PORT   # flash (set PORT to your board)
```

In the Arduino IDE: install the **FastLED** library, open this folder, select
**Arduino Uno** as the board, and upload.

## Making a change

1. **Fork** and create a topic branch off `main`.
2. Make your change. Keep the sketch compiling for the Uno — flash and (RAM)
   budget there is tight.
3. **Format** your code before committing:
   ```bash
   clang-format -i cockpit-ambient-lighting.ino
   ```
4. Update **docs** (`README.md`, `docs/`) if you changed wiring, the SimHub
   message format, or user-facing behaviour.
5. Add a line to **`CHANGELOG.md`** under `## [Unreleased]`.
6. Open a **pull request** and fill in the template.

## CI

Every PR is automatically:

- **Compiled** against the Arduino Uno R3 — this check must pass to merge.
- **Style-checked** with `clang-format` — informational, won't block your PR.

## Commit messages

Short, imperative summaries are great (`Add yellow-flag effect`,
`Fix off-by-one on last LED`). Conventional Commit prefixes
(`feat:`, `fix:`, `docs:`, `ci:`) are welcome but not required.

## Releases

Maintainers tag releases as `vX.Y.Z`. Pushing a `v*` tag triggers a build that
attaches the compiled `.hex` to a GitHub Release.

## Questions

Open a [discussion or issue](https://github.com/pixelsonly/cockpit-ambient-lighting/issues),
or check the [hardware](docs/HARDWARE.md) and [SimHub](docs/SIMHUB_SETUP.md) guides.
