#pragma once
#include "esp_err.h"

typedef void (*scanner_cb_t)(const char *id);

esp_err_t scanner_init(scanner_cb_t cb);
