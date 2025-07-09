#include "feeding.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "feeding";
static FILE *log_file = NULL;
static nvs_handle_t nvs_handle;
static int interval_days = 0; // default loaded from menuconfig

#ifndef CONFIG_FEEDING_INTERVAL_DAYS
#define CONFIG_FEEDING_INTERVAL_DAYS 7
#endif

esp_err_t feeding_init(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) return ret;

    ret = nvs_open("feed", NVS_READWRITE, &nvs_handle);
    if (ret != ESP_OK) return ret;

    int32_t days = CONFIG_FEEDING_INTERVAL_DAYS;
    nvs_get_i32(nvs_handle, "interval", &days);
    interval_days = days;

    log_file = fopen("/spiffs/feedings.csv", "a+");
    if (!log_file) {
        ESP_LOGE(TAG, "Failed to open feedings.csv");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t feeding_log(time_t timestamp, const char *prey, float weight_change, bool refusal)
{
    if (!log_file) return ESP_ERR_INVALID_STATE;
    fprintf(log_file, "%ld,%s,%.1f,%d\n", (long)timestamp, prey ? prey : "", weight_change, refusal ? 1 : 0);
    fflush(log_file);
    nvs_set_i64(nvs_handle, "last", (int64_t)timestamp);
    nvs_commit(nvs_handle);
    return ESP_OK;
}

esp_err_t feeding_get_last(time_t *timestamp, char *prey, size_t prey_len, float *weight_change, bool *refusal)
{
    if (!log_file) return ESP_ERR_INVALID_STATE;
    fseek(log_file, 0, SEEK_END);
    long size = ftell(log_file);
    if (size <= 0) return ESP_ERR_NOT_FOUND;

    long pos = size;
    int newline = 0;
    while (pos > 0) {
        pos--;
        fseek(log_file, pos, SEEK_SET);
        int c = fgetc(log_file);
        if (c == '\n') {
            if (newline) break;
            newline = 1;
        }
    }
    char line[128];
    fgets(line, sizeof(line), log_file);
    time_t ts; char prey_buf[64]; float wt; int ref;
    sscanf(line, "%ld,%63[^,],%f,%d", (long *)&ts, prey_buf, &wt, &ref);
    if (timestamp) *timestamp = ts;
    if (prey && prey_len) strncpy(prey, prey_buf, prey_len - 1), prey[prey_len - 1] = '\0';
    if (weight_change) *weight_change = wt;
    if (refusal) *refusal = ref != 0;
    return ESP_OK;
}

esp_err_t feeding_set_interval_days(int days)
{
    interval_days = days;
    nvs_set_i32(nvs_handle, "interval", days);
    nvs_commit(nvs_handle);
    return ESP_OK;
}

esp_err_t feeding_get_interval_days(int *days)
{
    if (days) *days = interval_days;
    return ESP_OK;
}

static time_t get_last_timestamp(void)
{
    int64_t val = 0;
    nvs_get_i64(nvs_handle, "last", &val);
    return (time_t)val;
}

bool feeding_overdue(int *days_until)
{
    time_t last = get_last_timestamp();
    time_t now = time(NULL);
    int diff = (int)((now - last) / 86400);
    if (days_until) *days_until = interval_days - diff;
    return diff >= interval_days;
}
