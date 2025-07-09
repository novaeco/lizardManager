#include "ds18b20.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ds18b20";
static gpio_num_t ds_pin = -1;
static bool ds_initialized = false;

/* 1-Wire timing constants (microseconds) */
#define OW_RESET_MIN_US   480
#define OW_RESET_WAIT_US   70
#define OW_WRITE_SLOT_US   65
#define OW_READ_SAMPLE_US  10
#define OW_RECOVERY_US     5
#define DS_CONVERT_MS    750

static esp_err_t ow_reset(void)
{
    gpio_set_direction(ds_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(ds_pin, 0);
    esp_rom_delay_us(OW_RESET_MIN_US);
    gpio_set_direction(ds_pin, GPIO_MODE_INPUT);
    esp_rom_delay_us(OW_RESET_WAIT_US);
    bool present = !gpio_get_level(ds_pin);
    esp_rom_delay_us(OW_RESET_MIN_US);
    return present ? ESP_OK : ESP_ERR_TIMEOUT;
}

static void ow_write_bit(int bit)
{
    gpio_set_direction(ds_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(ds_pin, 0);
    if (bit) {
        esp_rom_delay_us(OW_READ_SAMPLE_US);
        gpio_set_direction(ds_pin, GPIO_MODE_INPUT);
        esp_rom_delay_us(OW_WRITE_SLOT_US - OW_READ_SAMPLE_US);
    } else {
        esp_rom_delay_us(OW_WRITE_SLOT_US);
        gpio_set_direction(ds_pin, GPIO_MODE_INPUT);
        esp_rom_delay_us(OW_RECOVERY_US);
    }
}

static int ow_read_bit(void)
{
    gpio_set_direction(ds_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(ds_pin, 0);
    esp_rom_delay_us(OW_RECOVERY_US);
    gpio_set_direction(ds_pin, GPIO_MODE_INPUT);
    esp_rom_delay_us(OW_READ_SAMPLE_US);
    int bit = gpio_get_level(ds_pin);
    esp_rom_delay_us(OW_WRITE_SLOT_US - OW_READ_SAMPLE_US);
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

static uint8_t ds_crc8(const uint8_t *addr, uint8_t len)
{
    uint8_t crc = 0;
    while (len--) {
        uint8_t inbyte = *addr++;
        for (uint8_t i = 0; i < 8; i++) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
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
    ds_initialized = true;
    ESP_LOGI(TAG, "Initialized on GPIO %d", pin);
    return ESP_OK;
}

esp_err_t ds18b20_read(float *temperature)
{
    if (!ds_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    if (ow_reset() != ESP_OK) return ESP_ERR_TIMEOUT;
    ow_write_byte(0xCC); // skip ROM
    ow_write_byte(0x44); // convert T
    vTaskDelay(pdMS_TO_TICKS(DS_CONVERT_MS));

    if (ow_reset() != ESP_OK) return ESP_ERR_TIMEOUT;
    ow_write_byte(0xCC);
    ow_write_byte(0xBE); // read scratchpad

    uint8_t scratchpad[9] = {0};
    for (int i = 0; i < 9; i++) {
        scratchpad[i] = ow_read_byte();
    }
    ow_reset();

    if (ds_crc8(scratchpad, 8) != scratchpad[8]) {
        return ESP_ERR_INVALID_CRC;
    }

    int16_t raw = (scratchpad[1] << 8) | scratchpad[0];
    if (temperature) *temperature = raw / 16.0f;

    ESP_LOGI(TAG, "Read temp=%.1fC", temperature ? *temperature : 0);
    return ESP_OK;
}
