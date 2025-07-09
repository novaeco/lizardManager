#pragma once
#include "esp_err.h"
#include <time.h>
#include <stddef.h>

#define ENCLOSURES_MAX 16

typedef struct {
    char id[16];
    char name[32];
    char location[32];
    float temp_min;
    float temp_max;
    float hum_min;
    float hum_max;
    char sensors[64];
    time_t last_cleaning;
    int cleaning_interval_days;
    time_t last_substrate;
    int substrate_interval_days;
} enclosure_t;

esp_err_t enclosures_init(void);
esp_err_t enclosures_add(const enclosure_t *enc);
esp_err_t enclosures_get(const char *id, enclosure_t *out);
esp_err_t enclosures_update(const enclosure_t *enc);
esp_err_t enclosures_remove(const char *id);
esp_err_t enclosures_list(enclosure_t *out, size_t max, size_t *count);
bool enclosures_cleaning_due(const enclosure_t *enc, int *days_until);
bool enclosures_substrate_due(const enclosure_t *enc, int *days_until);
