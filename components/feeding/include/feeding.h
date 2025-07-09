#pragma once
#include "esp_err.h"
#include <stdbool.h>
#include <time.h>

esp_err_t feeding_init(void);
esp_err_t feeding_log(time_t timestamp, const char *prey, float weight_change, bool refusal);
esp_err_t feeding_get_last(time_t *timestamp, char *prey, size_t prey_len, float *weight_change, bool *refusal);
esp_err_t feeding_set_interval_days(int days);
esp_err_t feeding_get_interval_days(int *days);
bool feeding_overdue(int *days_until);
