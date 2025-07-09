#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "dht22.h"
#include "ds18b20.h"
#include "relay.h"
#include "logger.h"
#include "ui.h"
#include "settings.h"

static const char *TAG = "main";

static void cleanup(void)
{
    logger_close();
}

void app_main(void)
{
    ESP_LOGI(TAG, "Hello Lizard Manager!");

    settings_init();
    dht22_init(GPIO_NUM_4);
    ds18b20_init(GPIO_NUM_5);
    relay_init(GPIO_NUM_2);
    logger_init();
    esp_register_shutdown_handler(&cleanup);
    ui_init(NULL);

    float temp_min = 0, temp_max = 0, hum_min = 0, hum_max = 0;
    settings_get_temp_range(&temp_min, &temp_max);
    settings_get_hum_range(&hum_min, &hum_max);
    ESP_LOGI(TAG, "Temp range: %.1f-%.1fC Hum range: %.1f-%.1f%%", temp_min, temp_max, hum_min, hum_max);

    bool relay_on = false;

    while (true) {
        float t1 = 0.0f, h1 = 0.0f, t2 = 0.0f;
        dht22_read(&t1, &h1);
        ds18b20_read(&t2);
        ESP_LOGI(TAG, "DHT22: %.1fC %.1f%%  DS18B20: %.1fC", t1, h1, t2);
        logger_log(t1, h1, t2);
        ui_set_values(t1, h1);

        if ((t1 < temp_min) || (h1 < hum_min)) {
            if (!relay_on) {
                relay_set_state(true);
                relay_on = true;
            }
        } else if ((t1 > temp_max) || (h1 > hum_max)) {
            if (relay_on) {
                relay_set_state(false);
                relay_on = false;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }

    cleanup();
}
