# Components

This project defines several custom components:

- **dht22** - driver for reading temperature and humidity from a DHT22 sensor.
- **ds18b20** - 1-Wire driver with CRC checking for DS18B20 temperature probes.
- **animals** - simple database of registered animals.
- **relay** - simple GPIO-controlled relay driver.
- **logger** - saves sensor readings to a CSV file on SPIFFS.
- **ui** - minimal LVGL interface displaying temperature and humidity.
- **settings** - stores configurable temperature and humidity ranges in NVS.
- **genetics** - stores species and morph definitions and basic trait utilities.
- **feeding** - records feeding events and stores reminder settings.
- **health** - records weight, length and health events.
- **stock** - tracks feeder insect inventory and expiration dates.
- **ledger** - logs purchases and sales with counterparty details.
- **permits** - tracks permit renewal dates and inspection schedules.
