# Documentation

This directory stores project documentation.

See `french_eu_reptile_regs.md` for a summary of French and EU regulations on amateur reptile keeping.

## Logs and Interface

Sensor readings are continually appended to `/spiffs/readings.csv` on the
ESP32's SPIFFS partition. Use `idf.py spiffs_download` to copy the file from the
device.

When a display is connected, the LVGL-based UI shows current temperature and
humidity readings.
