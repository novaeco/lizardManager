#pragma once
#include "esp_err.h"

esp_err_t logger_init(void);
esp_err_t logger_log(float temp_c, float humidity, float probe_temp_c);
void logger_close(void);
