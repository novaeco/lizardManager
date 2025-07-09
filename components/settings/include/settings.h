#pragma once
#include "esp_err.h"

esp_err_t settings_init(void);
esp_err_t settings_get_temp_range(float *min_c, float *max_c);
esp_err_t settings_set_temp_range(float min_c, float max_c);
esp_err_t settings_get_hum_range(float *min_percent, float *max_percent);
esp_err_t settings_set_hum_range(float min_percent, float max_percent);
