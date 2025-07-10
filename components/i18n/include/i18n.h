#pragma once
#include "esp_err.h"

typedef enum {
    I18N_TEMP_STATIC,
    I18N_HUM_STATIC,
    I18N_LAST_FEED_STATIC,
    I18N_NEXT_FEED_STATIC,
    I18N_STATS_STATIC,
    I18N_TEMP,
    I18N_HUMIDITY,
    I18N_LAST_FEED,
    I18N_NEXT_FEED,
    I18N_STATS,
    I18N_KEY_COUNT
} i18n_key_t;

void i18n_init(void);
const char *i18n_str(i18n_key_t key);
