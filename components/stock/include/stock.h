#pragma once
#include "esp_err.h"
#include <time.h>
#include <stdbool.h>
#include <stddef.h>

#define STOCK_MAX 32

typedef struct {
    char name[32];
    int quantity;
    time_t expiration;
} stock_item_t;

esp_err_t stock_init(void);
esp_err_t stock_add(const stock_item_t *item);
esp_err_t stock_get(const char *name, stock_item_t *out);
esp_err_t stock_update(const stock_item_t *item);
esp_err_t stock_remove(const char *name);
esp_err_t stock_list(stock_item_t *out, size_t max, size_t *count);
bool stock_low(const char *name, int threshold);
bool stock_expiring_soon(const char *name, time_t now, int days);
