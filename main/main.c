#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "dht22.h"
#include "ds18b20.h"
#include "relay.h"
#include "logger.h"
#include "feeding.h"
#include "ledger.h"
#include "permits.h"
#include "auth.h"
#include "audit.h"
#include "ui.h"
#include "settings.h"
#include "backup.h"

static const char *TAG = "main";

static void cleanup(void)
{
    logger_close();
    audit_close();
}

void app_main(void)
{
    ESP_LOGI(TAG, "Hello Lizard Manager!");

    esp_err_t err;

    err = audit_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "audit_init failed: %s", esp_err_to_name(err));
    } else {
        audit_log("system", "boot");
    }

    err = auth_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "auth_init failed: %s", esp_err_to_name(err));
    }

    err = settings_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "settings_init failed: %s", esp_err_to_name(err));
        abort();
    }
    ESP_LOGI(TAG, "settings initialized");

    err = dht22_init(GPIO_NUM_4);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "dht22_init failed: %s", esp_err_to_name(err));
        abort();
    }

    err = ds18b20_init(GPIO_NUM_5);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ds18b20_init failed: %s", esp_err_to_name(err));
        abort();
    }

    err = relay_init(GPIO_NUM_2);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "relay_init failed: %s", esp_err_to_name(err));
        abort();
    }

    err = logger_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "logger_init failed: %s", esp_err_to_name(err));
        abort();
    }

    err = feeding_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "feeding_init failed: %s", esp_err_to_name(err));
        abort();
    }

    err = ledger_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ledger_init failed: %s", esp_err_to_name(err));
        abort();
    }

    err = permits_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "permits_init failed: %s", esp_err_to_name(err));
        abort();
    }

    err = backup_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "backup_init failed: %s", esp_err_to_name(err));
    } else {
        backup_schedule(CONFIG_BACKUP_INTERVAL_HOURS);
    }

    ESP_LOGI(TAG, "All components initialized");
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
        time_t last = 0;
        feeding_get_last(&last, NULL, 0, NULL, NULL);
        int days_left = 0;
        feeding_overdue(&days_left);
        ui_set_feeding(last, days_left);
        int feed_count = 0;
        float avg_interval = 0.0f;
        feeding_get_stats(&feed_count, &avg_interval);
        ui_set_stats(feed_count, avg_interval);

        permit_t list[PERMITS_MAX];
        size_t count = 0;
        permits_list(list, PERMITS_MAX, &count);
        for (size_t i = 0; i < count; ++i) {
            int days = 0;
            if (permits_due(&list[i], &days)) {
                ESP_LOGW(TAG, "Permit for %s expired %d days ago", list[i].animal_id, -days);
            } else if (days <= 30) {
                ESP_LOGI(TAG, "Permit for %s expires in %d days", list[i].animal_id, days);
            }
            if (permits_inspection_due(&list[i], &days)) {
                ESP_LOGW(TAG, "Inspection for %s overdue by %d days", list[i].animal_id, -days);
            }
        }

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
