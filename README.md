# Lizard Manager

Lizard Manager is an ESP-IDF based project for monitoring and controlling a reptile breeding environment. It targets ESP32 boards and aims to integrate sensors and actuators for temperature, humidity and lighting management.

## Directory Layout

- `main/` - application source code
- `components/` - additional components
- `config/` - configuration files
- `docs/` - project documentation

## Prerequisites

- **ESP-IDF**: version 5.0 or later
- **Hardware**: ESP32 development board (for example, ESP32-WROOM-32) with connected temperature/humidity sensors and actuators such as heaters or lights.

## Building and Running

1. Install ESP-IDF and set up the environment.
2. Run `idf.py set-target esp32` once for the project.
3. Build the firmware with `idf.py build`.
4. Flash and monitor with `idf.py flash monitor`.

## Roadmap

- Sensor integration for monitoring enclosure conditions
- Control loops to maintain temperature and humidity
- Simple user interface for configuration and monitoring
- Data logging and analytics features


## License

This project is licensed under the [MIT License](LICENSE).
