#include "unity.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "dht22.h"
#include "ds18b20.h"
#include "relay.h"
#include "logger.h"
#include "feeding.h"

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

void test_logger_requires_init(void)
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, logger_log(0.0f, 0.0f, 0.0f));
}

void test_logger_write_temp_fs(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, logger_init());
    TEST_ASSERT_EQUAL(ESP_OK, logger_log(1.0f, 2.0f, 3.0f));
    logger_close();
    FILE *f = fopen("/spiffs/readings.csv", "r");
    TEST_ASSERT_NOT_NULL(f);
    char line[32] = {0};
    fgets(line, sizeof(line), f);
    fclose(f);
    TEST_ASSERT_EQUAL_STRING("1.0,2.0,3.0\n", line);
}

void test_feeding_log(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, feeding_init());
    time_t now = 123456;
    TEST_ASSERT_EQUAL(ESP_OK, feeding_log(now, "mouse", 4.5f, false));
    time_t ts = 0; char prey[16] = {0}; float wt = 0.0f; bool ref = true;
    TEST_ASSERT_EQUAL(ESP_OK, feeding_get_last(&ts, prey, sizeof(prey), &wt, &ref));
    TEST_ASSERT_EQUAL(now, ts);
    TEST_ASSERT_EQUAL_STRING("mouse", prey);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 4.5f, wt);
    TEST_ASSERT_FALSE(ref);
}

void app_main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_dht22_requires_init);
    RUN_TEST(test_dht22_init_invalid_pin);
    RUN_TEST(test_dht22_init_valid_pin);

    RUN_TEST(test_ds18b20_requires_init);
    RUN_TEST(test_ds18b20_init_invalid_pin);
    RUN_TEST(test_ds18b20_init_valid_pin);

    RUN_TEST(test_relay_requires_init);
    RUN_TEST(test_relay_init_invalid_pin);
    RUN_TEST(test_relay_basic);

    RUN_TEST(test_logger_requires_init);
    RUN_TEST(test_logger_write_temp_fs);
    RUN_TEST(test_feeding_log);
    UNITY_END();
}
