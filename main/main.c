#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "dht22.h"
#include "ds18b20.h"
#include "relay.h"

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Hello Lizard Manager!");

    dht22_init(GPIO_NUM_4);
    ds18b20_init(GPIO_NUM_5);
    relay_init(GPIO_NUM_2);

    while (true) {
        float t1 = 0.0f, h1 = 0.0f, t2 = 0.0f;
        dht22_read(&t1, &h1);
        ds18b20_read(&t2);
        ESP_LOGI(TAG, "DHT22: %.1fC %.1f%%  DS18B20: %.1fC", t1, h1, t2);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
