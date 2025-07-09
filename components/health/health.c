#include "health.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "health";
static FILE *log_file = NULL;

esp_err_t health_init(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "SPIFFS mount failed: %s", esp_err_to_name(ret));
        return ret;
    }
    log_file = fopen("/spiffs/health.csv", "a+");
    if (!log_file) {
        ESP_LOGE(TAG, "Failed to open health log");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t health_log_measurement(const char *animal_id, time_t timestamp,
                                float weight_g, float length_cm)
{
    if (!log_file || !animal_id) return ESP_ERR_INVALID_STATE;
    fprintf(log_file, "%ld,%s,%.1f,%.1f\n", (long)timestamp, animal_id, weight_g, length_cm);
    fflush(log_file);
    return ESP_OK;
}

esp_err_t health_log_event(const char *animal_id, time_t timestamp,
                           const char *event_desc, const char *attachments)
{
    if (!log_file || !animal_id) return ESP_ERR_INVALID_STATE;
    fprintf(log_file, "%ld,%s,%s,%s\n", (long)timestamp, animal_id,
            event_desc ? event_desc : "",
            attachments ? attachments : "");
    fflush(log_file);
    return ESP_OK;
}
