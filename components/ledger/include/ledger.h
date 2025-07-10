#pragma once
#include "esp_err.h"
#include <time.h>
#include <stddef.h>

#define LEDGER_MAX 64

typedef enum {
    LEDGER_PURCHASE,
    LEDGER_SALE
} ledger_type_t;

typedef struct {
    time_t timestamp;
    ledger_type_t type;
    char item_id[16];
    char buyer[64];
    char seller[64];
    float price;
} ledger_entry_t;

esp_err_t ledger_init(void);
esp_err_t ledger_record(const ledger_entry_t *entry);
esp_err_t ledger_list(ledger_entry_t *out, size_t max, size_t *count);
