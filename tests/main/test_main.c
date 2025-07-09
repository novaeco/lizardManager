#include "unity.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "dht22.h"
#include "ds18b20.h"
#include "relay.h"

void setUp(void) {}
void tearDown(void) {}

void test_dht22_init_invalid_pin(void)
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, dht22_init(-1));
}

void test_dht22_init_valid_pin(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, dht22_init(GPIO_NUM_4));
}

void test_dht22_requires_init(void)
{
    float t, h;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, dht22_read(&t, &h));
}

void test_ds18b20_init_invalid_pin(void)
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ds18b20_init(GPIO_NUM_MAX));
}

void test_ds18b20_init_valid_pin(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, ds18b20_init(GPIO_NUM_5));
}

void test_ds18b20_requires_init(void)
{
    float t;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, ds18b20_read(&t));
}

void test_relay_init_invalid_pin(void)
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, relay_init(GPIO_NUM_MAX));
}

void test_relay_basic(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, relay_init(GPIO_NUM_18));
    TEST_ASSERT_EQUAL(ESP_OK, relay_set_state(false));
}

void test_relay_requires_init(void)
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, relay_set_state(true));
}

void app_main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_dht22_init_invalid_pin);
    RUN_TEST(test_dht22_init_valid_pin);
    RUN_TEST(test_dht22_requires_init);
    RUN_TEST(test_ds18b20_init_invalid_pin);
    RUN_TEST(test_ds18b20_init_valid_pin);
    RUN_TEST(test_ds18b20_requires_init);
    RUN_TEST(test_relay_init_invalid_pin);
    RUN_TEST(test_relay_basic);
    RUN_TEST(test_relay_requires_init);
    UNITY_END();
}
