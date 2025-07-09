# Lizard Manager

Lizard Manager is an ESP-IDF based project for monitoring and controlling a reptile breeding environment. It targets ESP32 boards and aims to integrate sensors and actuators for temperature, humidity and lighting management.

## Directory Layout

- `main/` - application source code
- `components/` - additional components
- `config/` - configuration files
- `docs/` - project documentation
- Wiring recommendations are available in `docs/pin_assignments.md`

## Prerequisites

- **ESP-IDF**: version 5.0 or later
- **Hardware**: ESP32 development board (for example, ESP32-WROOM-32) with connected temperature/humidity sensors and actuators such as heaters or lights.

## Building and Running

1. Install ESP-IDF and set up the environment.
2. Run `idf.py set-target esp32` once for the project.
3. Build the firmware with `idf.py build`.
   The UI component pulls LVGL version 8.3 automatically from Espressif's
   component registry during this step.
4. Flash and monitor with `idf.py flash monitor`.

## Configuration

A `sdkconfig.defaults` file at the repository root provides common settings for a generic ESP32 board, including UART speeds. Copy it to `sdkconfig` before building:

```bash
cp sdkconfig.defaults sdkconfig
```

Modify the copied file with `idf.py menuconfig` if your board needs different options. You can maintain multiple configuration files (for example, `sdkconfig.nodemcu`) and select one with `idf.py -DSDKCONFIG=<file> build`.

### Temperature and Humidity Ranges

Thresholds for activating the relay are stored in NVS and can be configured
with `idf.py menuconfig` under **Settings**. They may also be changed at runtime
using the `settings_set_temp_range` and `settings_set_hum_range` APIs.

## Roadmap

- Sensor integration for monitoring enclosure conditions
- Control loops to maintain temperature and humidity
- Simple user interface for configuration and monitoring
- Data logging and analytics features

## Logging

Sensor readings are saved to `/spiffs/readings.csv` on the device's SPIFFS
partition. You can retrieve the file over UART with
`idf.py spiffs_upload`/`spiffs_download` or by reading the partition using the
ESP-IDF SPIFFS utilities.

## User Interface

A small LVGL-based UI shows the current temperature and humidity on the
connected display. Use `idf.py menuconfig` to select an SPI or parallel
interface and to set the default screen resolution. The interface updates every
five seconds with the latest values from the sensors.

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for coding style guidelines, the pull request workflow, and instructions on running tests locally.

## License

This project is licensed under the [MIT License](LICENSE).
