# Components

This project defines several custom components:

- **dht22** - driver for reading temperature and humidity from a DHT22 sensor.
- **ds18b20** - 1-Wire driver with CRC checking for DS18B20 temperature probes.
- **relay** - simple GPIO-controlled relay driver.
- **logger** - saves sensor readings to a CSV file on SPIFFS.
- **ui** - minimal LVGL interface displaying temperature and humidity.
- **settings** - stores configurable temperature and humidity ranges in NVS.
