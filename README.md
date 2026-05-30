# Kame32

<p align="center">
  <img src="doc/kame32.jpg">
</p>

# Firmware

The `gamepad` PlatformIO environment serves both robot control and servo calibration:

- `/` opens the virtual gamepad.
- `/calibration` opens the servo calibration page.

The embedded pages use a lightweight, dependency-free version of the Prasheel UI visual language.

After uploading `gamepad`, connect to the `Kame32` Wi-Fi access point and open `http://kame32.local/` or the ESP32 access point IP.

If the normal upload drops the CP2102 serial port, use `gamepad_manual_upload`. Hold the ESP32 `BOOT` button, start upload, release `BOOT` after PlatformIO prints `Connecting...`.

# License 
This robot is licensed under a [Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0/).

<p align="center">
  <img src="https://mirrors.creativecommons.org/presskit/buttons/88x31/png/by-sa.png" width="200">
</p>
