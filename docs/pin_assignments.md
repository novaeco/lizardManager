# Hardware Pin Assignments

This document lists the default wiring used by Lizard Manager for common sensors, relay boards and SPI displays. Pin numbers can be changed in the firmware or via `menuconfig`.

## Sensors and Relay

| Device  | Module Pin | ESP32 GPIO | Notes |
|---------|------------|-----------|-------|
| DHT22   | DATA       | GPIO4     | Add a 4.7kΩ pull‑up to 3.3 V |
| DS18B20 | DATA       | GPIO5     | Requires a 4.7kΩ pull‑up |
| Relay   | IN         | GPIO2     | Use a module with 3.3 V logic |

## ILI9341 SPI Display

The default build expects an ILI9341-compatible screen on the SPI bus. MISO is optional if the display does not return data.

| Signal | ESP32 GPIO |
|--------|------------|
| MOSI   | GPIO23 |
| MISO   | GPIO19 |
| SCLK   | GPIO18 |
| CS     | GPIO5 |
| D/C    | GPIO16 |
| RESET  | GPIO17 |
| VCC    | 3.3 V |
| GND    | GND |

## Voltage Levels and Safety

The ESP32 uses 3.3 V logic. Connecting 5 V signals directly to its GPIO pins can permanently damage the chip. Use level shifters if your hardware requires higher voltages. When switching mains-powered equipment with the relay, keep high-voltage wiring separate from the ESP32 and sensors, and ensure the relay module is rated for the load.
