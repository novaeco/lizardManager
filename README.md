# Lizard Manager

Lizard Manager is an ESP-IDF based project for monitoring and controlling a reptile breeding environment. It targets ESP32 boards and aims to integrate sensors and actuators for temperature, humidity and lighting management.

## Directory Layout

- `main/` - application source code
- `components/` - additional components
- `config/` - configuration files
- `docs/` - project documentation
- Wiring recommendations are available in `docs/pin_assignments.md`
- A step-by-step hardware guide can be found in `docs/hardware_setup.md`

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
- Enclosure records with maintenance reminders

## Logging

Sensor readings are saved to `/spiffs/readings.csv` on the device's SPIFFS partition. Weight and health events are appended to `/spiffs/health.csv`. Purchase and sale transactions are stored in `/spiffs/ledger.csv`. Store related photos or documents in `/spiffs/attachments`. Retrieve files with `idf.py spiffs_upload`/`spiffs_download` or by reading the SPIFFS partition.
Use `tools/export_summary.py` to generate a PDF summary of the logs.
Use `tools/generate_pdf_docs.py` to create breeding, capacity, sales, and cession certificates in PDF format.
Use `tools/analyze_logs.py` to generate graphs showing growth curves and feeding frequency.

## Backups

Logs and configuration can be uploaded over Wi-Fi to a remote server. Set your
Wi-Fi credentials and server URL in `menuconfig` under **Backup**. Backups are
encrypted with AES using the provided key before being sent. Automatic backups
run every `CONFIG_BACKUP_INTERVAL_HOURS` hours, and you can trigger a manual
upload at runtime with `backup_manual()`.

## Archiving

Run `tools/export_encrypted.py` to create an encrypted archive of the logs and
records stored on the device. The script gathers the files from the SPIFFS
partition, creates a compressed tarball and encrypts it using `openssl` with
AES‑256. Provide the passphrase with the `--passphrase` option or set the
`EXPORT_PASSPHRASE` environment variable. The resulting `.tar.gz.enc` file can
be supplied to inspectors for regulatory compliance. Audit logs are rotated
daily as `audit_YYYYMMDD.csv` and remain immutable once written.

## User Interface

A small LVGL-based UI shows the current temperature and humidity on the
connected display. Use `idf.py menuconfig` to select an SPI or parallel
interface and to set the default screen resolution. The interface updates every
five seconds with the latest values from the sensors.
The screen also shows the last feeding date and days until the next reminder.
High-contrast and large-font modes can be enabled in `menuconfig` under **UI configuration**.
An optional scanner component provides hooks for RFID or QR-code readers so an animal's record can be opened quickly when a tag is scanned.
Translations for English, French, German and Spanish are built in. Select the preferred language in `menuconfig` under **Language**.

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for coding style guidelines, the pull request workflow, and instructions on running tests locally. By participating in this project you agree to abide by our [Code of Conduct](CODE_OF_CONDUCT.md).

## License

This project is licensed under the [MIT License](LICENSE).
