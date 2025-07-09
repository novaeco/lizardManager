#include "animals.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "animals";

static animal_t animals[ANIMALS_MAX];
static size_t animal_count = 0;
static bool initialized = false;

static esp_err_t load_file(void)
{
    FILE *f = fopen("/spiffs/animals.csv", "r");
    if (!f) {
        return ESP_OK; // no file yet
    }
    char line[256];
    while (fgets(line, sizeof(line), f) && animal_count < ANIMALS_MAX) {
        char *saveptr = NULL;
        char *token = strtok_r(line, ",\n", &saveptr);
        if (!token) continue;
        strncpy(animals[animal_count].id, token, sizeof(animals[animal_count].id)-1);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) strncpy(animals[animal_count].species, token, sizeof(animals[animal_count].species)-1);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) strncpy(animals[animal_count].sex, token, sizeof(animals[animal_count].sex)-1);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) strncpy(animals[animal_count].birthdate, token, sizeof(animals[animal_count].birthdate)-1);
        token = strtok_r(NULL, ",\n", &saveptr);
        if (token) strncpy(animals[animal_count].legal_status, token, sizeof(animals[animal_count].legal_status)-1);
        token = strtok_r(NULL, "\n", &saveptr);
        if (token) strncpy(animals[animal_count].photos, token, sizeof(animals[animal_count].photos)-1);
        animal_count++;
    }
    fclose(f);
    return ESP_OK;
}

static esp_err_t save_file(void)
{
    FILE *f = fopen("/spiffs/animals.csv", "w");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open animals.csv for write");
        return ESP_FAIL;
    }
    for (size_t i = 0; i < animal_count; ++i) {
        fprintf(f, "%s,%s,%s,%s,%s,%s\n",
                animals[i].id,
                animals[i].species,
                animals[i].sex,
                animals[i].birthdate,
                animals[i].legal_status,
                animals[i].photos);
    }
    fclose(f);
    return ESP_OK;
}

esp_err_t animals_init(void)
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

static int find_index(const char *id)
{
    for (size_t i = 0; i < animal_count; ++i) {
        if (strcmp(animals[i].id, id) == 0) return (int)i;
    }
    return -1;
}

esp_err_t animals_add(const animal_t *animal)
{
    if (!initialized || !animal) return ESP_ERR_INVALID_STATE;
    if (animal_count >= ANIMALS_MAX) return ESP_ERR_NO_MEM;
    if (find_index(animal->id) >= 0) return ESP_ERR_INVALID_ARG;
    animals[animal_count++] = *animal;
    return save_file();
}

esp_err_t animals_get(const char *id, animal_t *out)
{
    if (!initialized || !id || !out) return ESP_ERR_INVALID_ARG;
    int idx = find_index(id);
    if (idx < 0) return ESP_ERR_NOT_FOUND;
    *out = animals[idx];
    return ESP_OK;
}

esp_err_t animals_update(const animal_t *animal)
{
    if (!initialized || !animal) return ESP_ERR_INVALID_STATE;
    int idx = find_index(animal->id);
    if (idx < 0) return ESP_ERR_NOT_FOUND;
    animals[idx] = *animal;
    return save_file();
}

esp_err_t animals_remove(const char *id)
{
    if (!initialized || !id) return ESP_ERR_INVALID_ARG;
    int idx = find_index(id);
    if (idx < 0) return ESP_ERR_NOT_FOUND;
    for (size_t i = idx; i < animal_count - 1; ++i) {
        animals[i] = animals[i+1];
    }
    animal_count--;
    return save_file();
}

esp_err_t animals_list(animal_t *out, size_t max, size_t *count)
{
    if (!initialized || !out || !count) return ESP_ERR_INVALID_ARG;
    size_t n = (animal_count < max) ? animal_count : max;
    for (size_t i = 0; i < n; ++i) out[i] = animals[i];
    *count = animal_count;
    return ESP_OK;
}
