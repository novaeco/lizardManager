# Documentation

This directory stores project documentation.

See `pin_assignments.md` for wiring instructions. See `french_eu_reptile_regs.md` for a summary of French and EU regulations on amateur reptile keeping.

## Required Hardware

The following peripherals were used when developing the project:

- ESP32 development board (e.g. DevKitC or NodeMCU‑32S)
- DHT22 humidity/temperature sensor on **GPIO4**
- DS18B20 temperature probe on **GPIO5**
- Relay module driven by **GPIO2**
- ILI9341 compatible SPI display connected as shown in the wiring section below

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
Refer to `pin_assignments.md` for the default 8080 pin mapping.

The default firmware initializes an **ILI9341** compatible SPI display using the
`esp_lcd` driver provided by ESP-IDF, so no custom component is required.
Pin assignments can be customized in the source or via `menuconfig`.

You may also override the resolution at runtime by passing a
`ui_screen_config_t` structure to `ui_init`.

## Adjusting Temperature and Humidity Ranges

The relay controlling the heating or humidifying device switches based on
configurable thresholds stored in NVS. Default values can be set in
`menuconfig` under **Settings**. At runtime you can update the ranges over
UART using the `settings_set_*` functions from a custom command handler or
via `idf.py monitor`.

## ESP32 Pinouts

Below are simplified pinout diagrams for two commonly used development boards. Consult the manufacturer's documentation for complete details.

### ESP32-DevKitC (WROOM-32)

```
                +-------------------------+
 3V3     [ ] -- | 3V3       IO23      [ ] |
 EN      [ ] -- | EN        IO22      [ ] |
 IO36    [ ] -- | IO36      TX0       [ ] |
 IO39    [ ] -- | IO39      RX0       [ ] |
 IO34    [ ] -- | IO34      IO21      [ ] |
 IO35    [ ] -- | IO35      IO19      [ ] |
 IO32    [ ] -- | IO32      IO18      [ ] |
 IO33    [ ] -- | IO33      IO5       [ ] |
 GND     [ ] -- | GND       IO17      [ ] |
 VIN/5V  [ ] -- | 5V        IO16      [ ] |
                +-------------------------+
```

### NodeMCU‑32S

```
                +-------------------------+
 3V3     [ ] -- | 3V3       IO23      [ ] |
 EN      [ ] -- | EN        IO22      [ ] |
 IO36    [ ] -- | IO36      TX0       [ ] |
 IO39    [ ] -- | IO39      RX0       [ ] |
 IO34    [ ] -- | IO34      IO21      [ ] |
 IO35    [ ] -- | IO35      IO19      [ ] |
 IO32    [ ] -- | IO32      IO18      [ ] |
 IO33    [ ] -- | IO33      IO5       [ ] |
 GND     [ ] -- | GND       IO17      [ ] |
 VIN/5V  [ ] -- | 5V        IO16      [ ] |
                +-------------------------+
```

These diagrams show the general layout of pins. Only the power rails, UART and commonly used GPIO pins are included.

## Example Wiring

The default firmware assumes the following connections:

- **DHT22** data pin on **GPIO4**
- **DS18B20** data pin on **GPIO5**
- **Relay** control pin on **GPIO2**

If a display is attached, connect the SPI or parallel signals according to the board's pinout. Keep data wires short to avoid noise on the sensor lines.

### Display Wiring Example

When using the built-in ILI9341 driver the following pins are expected by default:

- **MOSI** on **GPIO23**
- **SCLK** on **GPIO18**
- **Chip Select** on **GPIO5**
- **Data/Command** on **GPIO16**
- **Reset** on **GPIO17**

The configuration assumes a 320x240 screen. Adjust the pins or resolution in
`menuconfig` under **UI configuration** if your hardware differs.

## Power and Safety Notes

The ESP32 operates at 3.3 V logic. Do not connect 5 V signals directly to GPIO pins. Use a level shifter for peripherals that require 5 V.

Large loads such as heating mats or lamps must be switched via a relay or MOSFET rated for the voltage and current of the device. Ensure all mains wiring is properly insulated and follow local electrical safety regulations.

A stable power supply is essential. Budget extra headroom for attached peripherals and avoid powering high-current devices from the ESP32's 3.3 V regulator.

## Adjusting Partition Sizes

The partition layout is defined in `partitions.csv` at the project root. This file specifies the factory app, OTA slot and SPIFFS storage regions. Edit the size column of each entry to change how much flash space is allocated. The build system uses this custom table via the `CONFIG_PARTITION_TABLE_CUSTOM_FILENAME` option in `sdkconfig.defaults`.
