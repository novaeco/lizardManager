# Hardware Setup

This guide summarizes how to wire the sensors, relay and power connections for a basic Lizard Manager installation.

## Required Parts

- ESP32 development board (DevKitC or equivalent)
- DHT22 humidity/temperature sensor
- DS18B20 temperature probe
- Single-channel relay module
- 5&nbsp;V power supply for the ESP32 and peripherals
- Assorted jumper wires

## Wiring Overview

The default firmware expects the following GPIO assignments:

| Device  | ESP32 GPIO | Notes                                  |
|---------|------------|----------------------------------------|
| DHT22   | GPIO4      | Add a 4.7&nbsp;kΩ pull-up resistor     |
| DS18B20 | GPIO5      | Requires a 4.7&nbsp;kΩ pull-up         |
| Relay   | GPIO2      | Module should accept 3.3&nbsp;V logic  |

Refer to `docs/pin_assignments.md` for display pins and optional peripherals.

### Power Connections

Feed 5&nbsp;V to the board's VIN/5&nbsp;V pin and connect GND to the power supply ground.
Sensors draw 3.3&nbsp;V from the ESP32; do not exceed this voltage on any GPIO pin.
Use a separate supply for high-current loads if necessary.

### Relay Wiring

The relay module switches the heating or lighting device. Connect the relay's
IN pin to GPIO2 and power the module according to its specifications.
Route mains wiring away from the ESP32 and ensure the relay is rated for the
load you intend to control.

