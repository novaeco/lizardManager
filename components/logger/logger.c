#include "logger.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include <stdio.h>
#include <unistd.h>

static const char *TAG = "logger";
static FILE *log_file = NULL;

esp_err_t logger_init(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 3,
        .format_if_mount_failed = true
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SPIFFS (%s)", esp_err_to_name(ret));
        return ret;
    }

    log_file = fopen("/spiffs/readings.csv", "a");
    if (!log_file) {
        ESP_LOGE(TAG, "Failed to open log file");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Logging to /spiffs/readings.csv");
    return ESP_OK;
}

esp_err_t logger_log(float temp_c, float humidity, float probe_temp_c)
{
    if (!log_file) {
        return ESP_ERR_INVALID_STATE;
    }
    fprintf(log_file, "%.1f,%.1f,%.1f\n", temp_c, humidity, probe_temp_c);
    fflush(log_file);
    return ESP_OK;
}

void logger_close(void)
{
    if (log_file) {
        fflush(log_file);
        fsync(fileno(log_file));
        fclose(log_file);
        log_file = NULL;
    }
    esp_vfs_spiffs_unregister(NULL);
}
