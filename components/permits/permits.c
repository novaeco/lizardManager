#include "permits.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "permits";

static permit_t permits[PERMITS_MAX];
static size_t permit_count = 0;
static bool initialized = false;

static esp_err_t load_file(void)
{
    FILE *f = fopen("/spiffs/permits.csv", "r");
    if (!f) {
        return ESP_OK;
    }
    char line[256];
    while (fgets(line, sizeof(line), f) && permit_count < PERMITS_MAX) {
        char *saveptr = NULL;
        char *token = strtok_r(line, ",\n", &saveptr);
        if (!token) continue;
        strncpy(permits[permit_count].animal_id, token,
                sizeof(permits[permit_count].animal_id) - 1);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) strncpy(permits[permit_count].permit_type, token,
                           sizeof(permits[permit_count].permit_type) - 1);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) permits[permit_count].issued = strtol(token, NULL, 10);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) permits[permit_count].expires = strtol(token, NULL, 10);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) permits[permit_count].last_inspection = strtol(token, NULL, 10);
        token = strtok_r(NULL, "\n", &saveptr);
        if (token) permits[permit_count].inspection_interval_days = atoi(token);
        permit_count++;
    }
    fclose(f);
    return ESP_OK;
}

static esp_err_t save_file(void)
{
    FILE *f = fopen("/spiffs/permits.csv", "w");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open permits.csv for write");
        return ESP_FAIL;
    }
    for (size_t i = 0; i < permit_count; ++i) {
        fprintf(f, "%s,%s,%ld,%ld,%ld,%d\n",
                permits[i].animal_id,
                permits[i].permit_type,
                (long)permits[i].issued,
                (long)permits[i].expires,
                (long)permits[i].last_inspection,
                permits[i].inspection_interval_days);
    }
    fclose(f);
    return ESP_OK;
}

esp_err_t permits_init(void)
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
    load_file();
    initialized = true;
    return ESP_OK;
}

static int find_index(const char *animal_id)
{
    for (size_t i = 0; i < permit_count; ++i) {
        if (strcmp(permits[i].animal_id, animal_id) == 0) return (int)i;
    }
    return -1;
}

esp_err_t permits_add(const permit_t *permit)
{
    if (!initialized || !permit) return ESP_ERR_INVALID_STATE;
    if (permit_count >= PERMITS_MAX) return ESP_ERR_NO_MEM;
    if (find_index(permit->animal_id) >= 0) return ESP_ERR_INVALID_ARG;
    permits[permit_count++] = *permit;
    return save_file();
}

esp_err_t permits_get(const char *animal_id, permit_t *out)
{
    if (!initialized || !animal_id || !out) return ESP_ERR_INVALID_ARG;
    int idx = find_index(animal_id);
    if (idx < 0) return ESP_ERR_NOT_FOUND;
    *out = permits[idx];
    return ESP_OK;
}

esp_err_t permits_update(const permit_t *permit)
{
    if (!initialized || !permit) return ESP_ERR_INVALID_STATE;
    int idx = find_index(permit->animal_id);
    if (idx < 0) return ESP_ERR_NOT_FOUND;
    permits[idx] = *permit;
    return save_file();
}

esp_err_t permits_remove(const char *animal_id)
{
    if (!initialized || !animal_id) return ESP_ERR_INVALID_ARG;
    int idx = find_index(animal_id);
    if (idx < 0) return ESP_ERR_NOT_FOUND;
    for (size_t i = idx; i < permit_count - 1; ++i) {
        permits[i] = permits[i + 1];
    }
    permit_count--;
    return save_file();
}

esp_err_t permits_list(permit_t *out, size_t max, size_t *count)
{
    if (!initialized || !out || !count) return ESP_ERR_INVALID_ARG;
    size_t n = (permit_count < max) ? permit_count : max;
    for (size_t i = 0; i < n; ++i) out[i] = permits[i];
    *count = permit_count;
    return ESP_OK;
}

static bool due_helper(time_t last, int interval, int *days_until)
{
    time_t now = time(NULL);
    int diff = (int)((now - last) / 86400);
    if (days_until) *days_until = interval - diff;
    return diff >= interval;
}

bool permits_due(const permit_t *permit, int *days_until)
{
    if (!permit) return false;
    time_t now = time(NULL);
    int diff = (int)((permit->expires - now) / 86400);
    if (days_until) *days_until = diff;
    return diff <= 0;
}

bool permits_inspection_due(const permit_t *permit, int *days_until)
{
    if (!permit || permit->inspection_interval_days <= 0) return false;
    return due_helper(permit->last_inspection,
                      permit->inspection_interval_days,
                      days_until);
}
