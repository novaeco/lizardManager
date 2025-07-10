#include "ledger.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "ledger";

static ledger_entry_t entries[LEDGER_MAX];
static size_t entry_count = 0;
static bool initialized = false;

static esp_err_t load_file(void)
{
    FILE *f = fopen("/spiffs/ledger.csv", "r");
    if (!f) return ESP_OK; // no file yet
    char line[256];
    while (fgets(line, sizeof(line), f) && entry_count < LEDGER_MAX) {
        char *saveptr = NULL;
        char *token = strtok_r(line, ",\n", &saveptr);
        if (!token) continue;
        entries[entry_count].timestamp = strtoll(token, NULL, 10);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) entries[entry_count].type = atoi(token) ? LEDGER_SALE : LEDGER_PURCHASE;
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) strncpy(entries[entry_count].item_id, token, sizeof(entries[entry_count].item_id) - 1);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) strncpy(entries[entry_count].buyer, token, sizeof(entries[entry_count].buyer) - 1);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) strncpy(entries[entry_count].seller, token, sizeof(entries[entry_count].seller) - 1);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) entries[entry_count].price = atof(token);
        entry_count++;
    }
    fclose(f);
    return ESP_OK;
}

static esp_err_t save_file(void)
{
    FILE *f = fopen("/spiffs/ledger.csv", "w");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open ledger.csv for write");
        return ESP_FAIL;
    }
    for (size_t i = 0; i < entry_count; ++i) {
        fprintf(f, "%lld,%d,%s,%s,%s,%.2f\n", (long long)entries[i].timestamp,
                entries[i].type == LEDGER_SALE, entries[i].item_id,
                entries[i].buyer, entries[i].seller, entries[i].price);
    }
    fclose(f);
    return ESP_OK;
}

esp_err_t ledger_init(void)
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

esp_err_t ledger_record(const ledger_entry_t *entry)
{
    if (!initialized || !entry) return ESP_ERR_INVALID_STATE;
    if (entry_count >= LEDGER_MAX) return ESP_ERR_NO_MEM;
    entries[entry_count++] = *entry;
    return save_file();
}

esp_err_t ledger_list(ledger_entry_t *out, size_t max, size_t *count)
{
    if (!initialized || !out || !count) return ESP_ERR_INVALID_ARG;
    size_t n = (entry_count < max) ? entry_count : max;
    for (size_t i = 0; i < n; ++i) out[i] = entries[i];
    *count = entry_count;
    return ESP_OK;
}
