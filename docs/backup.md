# Wi-Fi Backups

The backup system uploads `/spiffs/readings.csv` and configuration data to a remote server using Wi-Fi. Credentials and the server URL are configured via `idf.py menuconfig` under **Backup**.

Backups use AES encryption with the key specified in the configuration. The `backup_manual()` API triggers a one-time upload. The firmware also launches a background task that runs every `CONFIG_BACKUP_INTERVAL_HOURS` hours to upload new logs automatically.
