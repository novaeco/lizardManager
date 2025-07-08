#include "ds18b20.h"
#include "esp_log.h"

static const char *TAG = "ds18b20";
static gpio_num_t ds_pin = -1;

esp_err_t ds18b20_init(gpio_num_t pin)
{
    ds_pin = pin;
    ESP_LOGI(TAG, "Initialized on GPIO %d", pin);
    return ESP_OK;
}

esp_err_t ds18b20_read(float *temperature)
{
    if (ds_pin < 0) {
        return ESP_ERR_INVALID_STATE;
    }
    if (temperature) *temperature = 26.5f;
    ESP_LOGI(TAG, "Read temp=%.1fC", *temperature);
    return ESP_OK;
}
