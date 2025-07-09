#include "unity.h"
#include "dht22.h"
#include "ds18b20.h"
#include "relay.h"
#include "driver/gpio.h"

void setUp(void) {}
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

void app_main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_dht22_read);
    RUN_TEST(test_dht22_invalid_state);
    RUN_TEST(test_ds18b20_read);
    RUN_TEST(test_ds18b20_invalid_state);
    RUN_TEST(test_relay_set_state);
    UNITY_END();
}
