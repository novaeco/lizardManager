#pragma once
#include "esp_err.h"

esp_err_t backup_init(void);
esp_err_t backup_manual(void);
esp_err_t backup_schedule(uint32_t hours);
