#include "enclosures.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "enclosures";
static enclosure_t enclosures[ENCLOSURES_MAX];
static size_t enclosure_count = 0;
static bool initialized = false;

static esp_err_t load_file(void)
{
    FILE *f = fopen("/spiffs/enclosures.csv", "r");
    if (!f) {
        return ESP_OK;
    }
    char line[256];
    while (fgets(line, sizeof(line), f) && enclosure_count < ENCLOSURES_MAX) {
        char *saveptr = NULL;
        char *token = strtok_r(line, ",\n", &saveptr);
        if (!token) continue;
        strncpy(enclosures[enclosure_count].id, token,
                sizeof(enclosures[enclosure_count].id) - 1);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) strncpy(enclosures[enclosure_count].name, token,
                           sizeof(enclosures[enclosure_count].name) - 1);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) strncpy(enclosures[enclosure_count].location, token,
                           sizeof(enclosures[enclosure_count].location) - 1);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) enclosures[enclosure_count].temp_min = strtof(token, NULL);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) enclosures[enclosure_count].temp_max = strtof(token, NULL);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) enclosures[enclosure_count].hum_min = strtof(token, NULL);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) enclosures[enclosure_count].hum_max = strtof(token, NULL);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) strncpy(enclosures[enclosure_count].sensors, token,
                           sizeof(enclosures[enclosure_count].sensors) - 1);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) enclosures[enclosure_count].last_cleaning = strtol(token, NULL, 10);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) enclosures[enclosure_count].cleaning_interval_days = atoi(token);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) enclosures[enclosure_count].last_substrate = strtol(token, NULL, 10);
        token = strtok_r(NULL, "\n", &saveptr);
        if (token) enclosures[enclosure_count].substrate_interval_days = atoi(token);
        enclosure_count++;
    }
    fclose(f);
    return ESP_OK;
}

static esp_err_t save_file(void)
{
    FILE *f = fopen("/spiffs/enclosures.csv", "w");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open enclosures.csv for write");
        return ESP_FAIL;
    }
    for (size_t i = 0; i < enclosure_count; ++i) {
        fprintf(f, "%s,%s,%s,%.1f,%.1f,%.1f,%.1f,%s,%ld,%d,%ld,%d\n",
                enclosures[i].id,
                enclosures[i].name,
                enclosures[i].location,
                enclosures[i].temp_min,
                enclosures[i].temp_max,
                enclosures[i].hum_min,
                enclosures[i].hum_max,
                enclosures[i].sensors,
                (long)enclosures[i].last_cleaning,
                enclosures[i].cleaning_interval_days,
                (long)enclosures[i].last_substrate,
                enclosures[i].substrate_interval_days);
    }
    fclose(f);
    return ESP_OK;
}

esp_err_t enclosures_init(void)
{
    if (initialized) return ESP_OK;
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
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) return ret;
    load_file();
    initialized = true;
    return ESP_OK;
}

static int find_index(const char *id)
{
    for (size_t i = 0; i < enclosure_count; ++i) {
        if (strcmp(enclosures[i].id, id) == 0) return (int)i;
    }
    return -1;
}

esp_err_t enclosures_add(const enclosure_t *enc)
{
    if (!initialized || !enc) return ESP_ERR_INVALID_STATE;
    if (enclosure_count >= ENCLOSURES_MAX) return ESP_ERR_NO_MEM;
    if (find_index(enc->id) >= 0) return ESP_ERR_INVALID_ARG;
    enclosures[enclosure_count++] = *enc;
    return save_file();
}

esp_err_t enclosures_get(const char *id, enclosure_t *out)
{
    if (!initialized || !id || !out) return ESP_ERR_INVALID_ARG;
    int idx = find_index(id);
    if (idx < 0) return ESP_ERR_NOT_FOUND;
    *out = enclosures[idx];
    return ESP_OK;
}

esp_err_t enclosures_update(const enclosure_t *enc)
{
    if (!initialized || !enc) return ESP_ERR_INVALID_STATE;
    int idx = find_index(enc->id);
    if (idx < 0) return ESP_ERR_NOT_FOUND;
    enclosures[idx] = *enc;
    return save_file();
}

esp_err_t enclosures_remove(const char *id)
{
    if (!initialized || !id) return ESP_ERR_INVALID_ARG;
    int idx = find_index(id);
    if (idx < 0) return ESP_ERR_NOT_FOUND;
    for (size_t i = idx; i < enclosure_count - 1; ++i) {
        enclosures[i] = enclosures[i + 1];
    }
    enclosure_count--;
    return save_file();
}

esp_err_t enclosures_list(enclosure_t *out, size_t max, size_t *count)
{
    if (!initialized || !out || !count) return ESP_ERR_INVALID_ARG;
    size_t n = (enclosure_count < max) ? enclosure_count : max;
    for (size_t i = 0; i < n; ++i) out[i] = enclosures[i];
    *count = enclosure_count;
    return ESP_OK;
}

static bool due_helper(time_t last, int interval, int *days_until)
{
    time_t now = time(NULL);
    int diff = (int)((now - last) / 86400);
    if (days_until) *days_until = interval - diff;
    return diff >= interval;
}

bool enclosures_cleaning_due(const enclosure_t *enc, int *days_until)
{
    if (!enc) return false;
    return due_helper(enc->last_cleaning, enc->cleaning_interval_days, days_until);
}

bool enclosures_substrate_due(const enclosure_t *enc, int *days_until)
{
    if (!enc) return false;
    return due_helper(enc->last_substrate, enc->substrate_interval_days, days_until);
}
