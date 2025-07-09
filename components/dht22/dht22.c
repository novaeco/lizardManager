#include "dht22.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"

static const char *TAG = "dht22";
static gpio_num_t dht22_pin = -1;
static bool dht22_initialized = false;

/* Timings in microseconds for the DHT22 protocol */
#define DHT22_START_US        1100
#define DHT22_RESPONSE_US       80
#define DHT22_BIT_THRESHOLD_US  40

static esp_err_t wait_level(int level, uint32_t timeout_us)
{
    int64_t start = esp_timer_get_time();
    while (gpio_get_level(dht22_pin) != level) {
        if ((esp_timer_get_time() - start) > timeout_us) {
            return ESP_ERR_TIMEOUT;
        }
    }
    return ESP_OK;
}

esp_err_t dht22_init(gpio_num_t pin)
{
    if (pin < 0 || pin >= GPIO_NUM_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    dht22_pin = pin;
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 1);
    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
    dht22_initialized = true;
    ESP_LOGI(TAG, "Initialized on GPIO %d", pin);
    return ESP_OK;
}

esp_err_t dht22_read(float *temperature, float *humidity)
{
    if (!dht22_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t data[5] = {0};

    // start signal
    gpio_set_direction(dht22_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(dht22_pin, 0);
    esp_rom_delay_us(DHT22_START_US);
    gpio_set_level(dht22_pin, 1);
    esp_rom_delay_us(DHT22_RESPONSE_US / 2);
    gpio_set_direction(dht22_pin, GPIO_MODE_INPUT);

    if (wait_level(0, DHT22_RESPONSE_US) != ESP_OK ||
        wait_level(1, DHT22_RESPONSE_US) != ESP_OK)
        return ESP_ERR_TIMEOUT;

    for (int i = 0; i < 40; i++) {
        if (wait_level(0, DHT22_RESPONSE_US) != ESP_OK) return ESP_ERR_TIMEOUT;
        if (wait_level(1, DHT22_RESPONSE_US) != ESP_OK) return ESP_ERR_TIMEOUT;
        int64_t start = esp_timer_get_time();
        if (wait_level(0, DHT22_RESPONSE_US) != ESP_OK) return ESP_ERR_TIMEOUT;
        int64_t len = esp_timer_get_time() - start;
        data[i/8] <<= 1;
        if (len > DHT22_BIT_THRESHOLD_US) data[i/8] |= 1;
    }

    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4]) {
        return ESP_ERR_INVALID_CRC;
    }

    int16_t raw_h = (data[0] << 8) | data[1];
    int16_t raw_t = (data[2] << 8) | data[3];
    if (raw_t & 0x8000) raw_t = -(raw_t & 0x7FFF);

    if (humidity) *humidity = raw_h / 10.0f;
    if (temperature) *temperature = raw_t / 10.0f;

    ESP_LOGI(TAG, "Read temp=%.1fC hum=%.1f%%", temperature ? *temperature : 0, humidity ? *humidity : 0);
    return ESP_OK;
}
