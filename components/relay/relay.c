#include "relay.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "relay";
static gpio_num_t relay_pin = -1;
static bool relay_initialized = false;

esp_err_t relay_init(gpio_num_t pin)
{
    if (pin < 0 || pin >= GPIO_NUM_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    relay_pin = pin;
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << pin,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    esp_err_t ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GPIO config failed: %s", esp_err_to_name(ret));
        return ret;
    }
    relay_initialized = true;
    ESP_LOGI(TAG, "Initialized on GPIO %d", pin);
    return ESP_OK;
}

esp_err_t relay_set_state(bool on)
{
    if (!relay_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    gpio_set_level(relay_pin, on ? 1 : 0);
    ESP_LOGI(TAG, "State %s", on ? "ON" : "OFF");
    return ESP_OK;
}
