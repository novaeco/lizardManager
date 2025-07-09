#include "unity.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include <stdbool.h>

/*
 * Simple simulation layer for sensor and relay APIs. The real hardware
 * drivers are not used during unit testing. Instead we provide lightweight
 * implementations that mimic their behaviour so tests can run on the host
 * without needing an ESP device attached.
 */

static bool dht22_initialized;
static bool ds18b20_initialized;
static bool relay_initialized;
static bool relay_state;

esp_err_t dht22_init(gpio_num_t pin)
{
    (void)pin;
    dht22_initialized = true;
    return ESP_OK;
}

esp_err_t dht22_read(float *temperature, float *humidity)
{
    if (!dht22_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    if (temperature) *temperature = 23.0f;
    if (humidity) *humidity = 50.0f;
    return ESP_OK;
}

esp_err_t ds18b20_init(gpio_num_t pin)
{
    (void)pin;
    ds18b20_initialized = true;
    return ESP_OK;
}

esp_err_t ds18b20_read(float *temperature)
{
    if (!ds18b20_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    if (temperature) *temperature = 26.0f;
    return ESP_OK;
}

esp_err_t relay_init(gpio_num_t pin)
{
    (void)pin;
    relay_initialized = true;
    relay_state = false;
    return ESP_OK;
}

esp_err_t relay_set_state(bool on)
{
    if (!relay_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    relay_state = on;
    return ESP_OK;
}

void setUp(void)
{
    dht22_initialized = false;
    ds18b20_initialized = false;
    relay_initialized = false;
    relay_state = false;
}
void tearDown(void) {}

void test_dht22_read(void)
{
    dht22_init(GPIO_NUM_4);
    float t = 0, h = 0;
    TEST_ASSERT_EQUAL(ESP_OK, dht22_read(&t, &h));
    TEST_ASSERT_TRUE(t > -40 && t < 80);
    TEST_ASSERT_TRUE(h >= 0 && h <= 100);
}

void test_dht22_invalid_state(void)
{
    float t, h;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, dht22_read(&t, &h));
}

void test_ds18b20_read(void)
{
    ds18b20_init(GPIO_NUM_5);
    float t = 0;
    TEST_ASSERT_EQUAL(ESP_OK, ds18b20_read(&t));
    TEST_ASSERT_TRUE(t > -55 && t < 125);
}

void test_ds18b20_invalid_state(void)
{
    float t;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, ds18b20_read(&t));
}

void test_relay_set_state(void)
{
    relay_init(GPIO_NUM_2);
    TEST_ASSERT_EQUAL(ESP_OK, relay_set_state(true));
    TEST_ASSERT_EQUAL(ESP_OK, relay_set_state(false));
}

void test_relay_set_state_invalid_state(void)
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, relay_set_state(true));
}

void app_main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_dht22_read);
    RUN_TEST(test_dht22_invalid_state);
    RUN_TEST(test_ds18b20_read);
    RUN_TEST(test_ds18b20_invalid_state);
    RUN_TEST(test_relay_set_state);
    RUN_TEST(test_relay_set_state_invalid_state);
    UNITY_END();
}
