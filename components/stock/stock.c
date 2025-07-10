#include "stock.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "stock";

static stock_item_t items[STOCK_MAX];
static size_t item_count = 0;
static bool initialized = false;

static esp_err_t load_file(void)
{
    FILE *f = fopen("/spiffs/stock.csv", "r");
    if (!f) return ESP_OK; // no file yet
    char line[128];
    while (fgets(line, sizeof(line), f) && item_count < STOCK_MAX) {
        char *saveptr = NULL;
        char *token = strtok_r(line, ",\n", &saveptr);
        if (!token) continue;
        strncpy(items[item_count].name, token,
                sizeof(items[item_count].name) - 1);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) items[item_count].quantity = atoi(token);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) items[item_count].expiration = strtoll(token, NULL, 10);
        item_count++;
    }
    fclose(f);
    return ESP_OK;
}

static esp_err_t save_file(void)
{
    FILE *f = fopen("/spiffs/stock.csv", "w");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open stock.csv for write");
        return ESP_FAIL;
    }
    for (size_t i = 0; i < item_count; ++i) {
        fprintf(f, "%s,%d,%lld\n", items[i].name, items[i].quantity,
                (long long)items[i].expiration);
    }
    fclose(f);
    return ESP_OK;
}

esp_err_t stock_init(void)
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

static int find_index(const char *name)
{
    for (size_t i = 0; i < item_count; ++i) {
        if (strcmp(items[i].name, name) == 0) return (int)i;
    }
    return -1;
}

esp_err_t stock_add(const stock_item_t *item)
{
    if (!initialized || !item) return ESP_ERR_INVALID_STATE;
    if (item_count >= STOCK_MAX) return ESP_ERR_NO_MEM;
    if (find_index(item->name) >= 0) return ESP_ERR_INVALID_ARG;
    items[item_count++] = *item;
    return save_file();
}

esp_err_t stock_get(const char *name, stock_item_t *out)
{
    if (!initialized || !name || !out) return ESP_ERR_INVALID_ARG;
    int idx = find_index(name);
    if (idx < 0) return ESP_ERR_NOT_FOUND;
    *out = items[idx];
    return ESP_OK;
}

esp_err_t stock_update(const stock_item_t *item)
{
    if (!initialized || !item) return ESP_ERR_INVALID_STATE;
    int idx = find_index(item->name);
    if (idx < 0) return ESP_ERR_NOT_FOUND;
    items[idx] = *item;
    return save_file();
}

esp_err_t stock_remove(const char *name)
{
    if (!initialized || !name) return ESP_ERR_INVALID_ARG;
    int idx = find_index(name);
    if (idx < 0) return ESP_ERR_NOT_FOUND;
    for (size_t i = idx; i < item_count - 1; ++i) items[i] = items[i+1];
    item_count--;
    return save_file();
}

esp_err_t stock_list(stock_item_t *out, size_t max, size_t *count)
{
    if (!initialized || !out || !count) return ESP_ERR_INVALID_ARG;
    size_t n = (item_count < max) ? item_count : max;
    for (size_t i = 0; i < n; ++i) out[i] = items[i];
    *count = item_count;
    return ESP_OK;
}

bool stock_low(const char *name, int threshold)
{
    if (!initialized || !name) return false;
    int idx = find_index(name);
    if (idx < 0) return false;
    return items[idx].quantity <= threshold;
}

bool stock_expiring_soon(const char *name, time_t now, int days)
{
    if (!initialized || !name) return false;
    int idx = find_index(name);
    if (idx < 0) return false;
    time_t limit = now + (time_t)days * 86400;
    return items[idx].expiration <= limit;
}
