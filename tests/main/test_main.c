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
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 25.0f, t);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 60.0f, h);
}

void test_ds18b20_read(void)
{
    ds18b20_init(GPIO_NUM_5);
    float t = 0;
    TEST_ASSERT_EQUAL(ESP_OK, ds18b20_read(&t));
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 26.5f, t);
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
    RUN_TEST(test_ds18b20_read);
    RUN_TEST(test_relay_set_state);
    UNITY_END();
}
