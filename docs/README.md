# Documentation

This directory stores project documentation.

See `french_eu_reptile_regs.md` for a summary of French and EU regulations on amateur reptile keeping.

## Logs and Interface

Sensor readings are continually appended to `/spiffs/readings.csv` on the
ESP32's SPIFFS partition. Use `idf.py spiffs_download` to copy the file from the
device.

When a display is connected, the LVGL-based UI shows current temperature and
humidity readings.

## Connecting a Display

The UI component supports both SPI and 8080 parallel displays. Select the
desired interface and default resolution in `menuconfig` under **UI
configuration**.

For SPI screens connect MOSI, MISO, SCLK, CS and D/C to the ESP32's SPI bus.
Parallel displays require wiring the D0–D7 data lines and the control pins
(WR, RD, CS and D/C). Use level shifting if your hardware operates at 5 V.

You may also override the resolution at runtime by passing a
`ui_screen_config_t` structure to `ui_init`.
