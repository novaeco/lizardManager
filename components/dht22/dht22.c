#include "dht22.h"
#include "esp_log.h"

static const char *TAG = "dht22";
static gpio_num_t dht22_pin = -1;

esp_err_t dht22_init(gpio_num_t pin)
{
    dht22_pin = pin;
    // Normally GPIO setup would go here
    ESP_LOGI(TAG, "Initialized on GPIO %d", pin);
    return ESP_OK;
}

esp_err_t dht22_read(float *temperature, float *humidity)
{
    if (dht22_pin < 0) {
        return ESP_ERR_INVALID_STATE;
    }
    // Stub reading - replace with real sensor code
    if (temperature) *temperature = 25.0f;
    if (humidity) *humidity = 60.0f;
    ESP_LOGI(TAG, "Read temp=%.1fC hum=%.1f%%", *temperature, *humidity);
    return ESP_OK;
}
