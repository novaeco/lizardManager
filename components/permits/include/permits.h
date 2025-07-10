#pragma once
#include "esp_err.h"
#include <stdbool.h>
#include <time.h>
#include <stddef.h>

#define PERMITS_MAX 32

typedef struct {
    char animal_id[16];
    char permit_type[16];
    time_t issued;
    time_t expires;
    time_t last_inspection;
    int inspection_interval_days;
} permit_t;

esp_err_t permits_init(void);
esp_err_t permits_add(const permit_t *permit);
esp_err_t permits_get(const char *animal_id, permit_t *out);
esp_err_t permits_update(const permit_t *permit);
esp_err_t permits_remove(const char *animal_id);
esp_err_t permits_list(permit_t *out, size_t max, size_t *count);
bool permits_due(const permit_t *permit, int *days_until);
bool permits_inspection_due(const permit_t *permit, int *days_until);
