#include "audit.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include <stdio.h>
#include <time.h>

static const char *TAG = "audit";
static FILE *audit_file = NULL;
static int current_day = -1;
static char audit_filename[32];

esp_err_t audit_init(void)
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
    time_t now = time(NULL);
    struct tm tm_info;
    localtime_r(&now, &tm_info);
    current_day = tm_info.tm_yday;
    snprintf(audit_filename, sizeof(audit_filename),
             "/spiffs/audit_%04d%02d%02d.csv",
             tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday);

    audit_file = fopen(audit_filename, "a");
    if (!audit_file) {
        ESP_LOGE(TAG, "Failed to open %s", audit_filename);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t audit_log(const char *username, const char *action)
{
    if (!audit_file) return ESP_ERR_INVALID_STATE;
    time_t now = time(NULL);
    struct tm tm_info;
    localtime_r(&now, &tm_info);
    if (tm_info.tm_yday != current_day) {
        fflush(audit_file);
        fclose(audit_file);
        current_day = tm_info.tm_yday;
        snprintf(audit_filename, sizeof(audit_filename),
                 "/spiffs/audit_%04d%02d%02d.csv",
                 tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday);
        audit_file = fopen(audit_filename, "a");
        if (!audit_file) {
            ESP_LOGE(TAG, "Failed to open %s", audit_filename);
            return ESP_FAIL;
        }
    }
    fprintf(audit_file, "%ld,%s,%s\n", (long)now,
            username ? username : "", action ? action : "");
    fflush(audit_file);
    return ESP_OK;
}

void audit_close(void)
{
    if (audit_file) {
        fflush(audit_file);
        fclose(audit_file);
        audit_file = NULL;
    }
}
