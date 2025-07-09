#include "ds18b20.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ds18b20";
static gpio_num_t ds_pin = -1;

static esp_err_t ow_reset(void)
{
    gpio_set_direction(ds_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(ds_pin, 0);
    esp_rom_delay_us(480);
    gpio_set_direction(ds_pin, GPIO_MODE_INPUT);
    esp_rom_delay_us(70);
    bool present = !gpio_get_level(ds_pin);
    esp_rom_delay_us(410);
    return present ? ESP_OK : ESP_ERR_TIMEOUT;
}

static void ow_write_bit(int bit)
{
    gpio_set_direction(ds_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(ds_pin, 0);
    if (bit) {
        esp_rom_delay_us(10);
        gpio_set_direction(ds_pin, GPIO_MODE_INPUT);
        esp_rom_delay_us(55);
    } else {
        esp_rom_delay_us(65);
        gpio_set_direction(ds_pin, GPIO_MODE_INPUT);
        esp_rom_delay_us(5);
    }
}

static int ow_read_bit(void)
{
    gpio_set_direction(ds_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(ds_pin, 0);
    esp_rom_delay_us(3);
    gpio_set_direction(ds_pin, GPIO_MODE_INPUT);
    esp_rom_delay_us(10);
    int bit = gpio_get_level(ds_pin);
    esp_rom_delay_us(53);
    return bit;
}

static void ow_write_byte(uint8_t v)
{
    for (int i = 0; i < 8; i++) {
        ow_write_bit((v >> i) & 1);
    }
}

static uint8_t ow_read_byte(void)
{
    uint8_t val = 0;
    for (int i = 0; i < 8; i++) {
        val |= ow_read_bit() << i;
    }
    return val;
}

esp_err_t ds18b20_init(gpio_num_t pin)
{
    if (pin < 0 || pin >= GPIO_NUM_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    ds_pin = pin;
    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
    esp_err_t ret = ow_reset();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Sensor not responding");
        return ret;
    }
    ESP_LOGI(TAG, "Initialized on GPIO %d", pin);
    return ESP_OK;
}

esp_err_t ds18b20_read(float *temperature)
{
    if (ds_pin < 0) {
        return ESP_ERR_INVALID_STATE;
    }

    if (ow_reset() != ESP_OK) return ESP_ERR_TIMEOUT;
    ow_write_byte(0xCC); // skip ROM
    ow_write_byte(0x44); // convert T
    vTaskDelay(pdMS_TO_TICKS(750));

    if (ow_reset() != ESP_OK) return ESP_ERR_TIMEOUT;
    ow_write_byte(0xCC);
    ow_write_byte(0xBE); // read scratchpad

    uint8_t lsb = ow_read_byte();
    uint8_t msb = ow_read_byte();
    ow_reset();

    int16_t raw = (msb << 8) | lsb;
    if (temperature) *temperature = raw / 16.0f;

    ESP_LOGI(TAG, "Read temp=%.1fC", temperature ? *temperature : 0);
    return ESP_OK;
}
