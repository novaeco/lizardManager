#include "ui.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "ui";
static lv_obj_t *temp_label;
static lv_obj_t *hum_label;

void ui_init(void)
{
    lv_init();
    temp_label = lv_label_create(lv_scr_act());
    hum_label = lv_label_create(lv_scr_act());
    lv_obj_align(temp_label, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_align(hum_label, LV_ALIGN_TOP_MID, 0, 30);
    lv_label_set_text(temp_label, "Temp: --.-C");
    lv_label_set_text(hum_label, "Humidity: --.-%");
    ESP_LOGI(TAG, "UI initialized");
}

void ui_set_values(float temp, float humidity)
{
    if (!temp_label || !hum_label) return;
    char buf[32];
    snprintf(buf, sizeof(buf), "Temp: %.1fC", temp);
    lv_label_set_text(temp_label, buf);
    snprintf(buf, sizeof(buf), "Humidity: %.1f%%", humidity);
    lv_label_set_text(hum_label, buf);
    lv_timer_handler();
}
