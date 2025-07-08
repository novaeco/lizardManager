#pragma once
#include "esp_err.h"
#include "driver/gpio.h"

esp_err_t ds18b20_init(gpio_num_t pin);
esp_err_t ds18b20_read(float *temperature);
