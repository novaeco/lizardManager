#pragma once
#include "esp_err.h"
#include "driver/gpio.h"

esp_err_t dht22_init(gpio_num_t pin);
esp_err_t dht22_read(float *temperature, float *humidity);
