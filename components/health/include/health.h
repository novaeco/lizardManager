#pragma once
#include "esp_err.h"
#include <time.h>

esp_err_t health_init(void);
esp_err_t health_log_measurement(const char *animal_id, time_t timestamp,
                                float weight_g, float length_cm);
esp_err_t health_log_event(const char *animal_id, time_t timestamp,
                           const char *event_desc, const char *attachments);
