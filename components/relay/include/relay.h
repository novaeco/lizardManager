#pragma once
#include "esp_err.h"
#include "driver/gpio.h"
#include <stdbool.h>

esp_err_t relay_init(gpio_num_t pin);
esp_err_t relay_set_state(bool on);
