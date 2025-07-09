#include "ui.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include <stdio.h>

static const char *TAG = "ui";
static lv_obj_t *temp_label;
static lv_obj_t *hum_label;

#if CONFIG_UI_DISPLAY_INTERFACE_SPI
static void display_board_init(void)
{
    ESP_LOGI(TAG, "Initializing SPI display (%dx%d)", CONFIG_UI_DISPLAY_WIDTH, CONFIG_UI_DISPLAY_HEIGHT);
    /* TODO: Add SPI display driver initialization here */
}
#elif CONFIG_UI_DISPLAY_INTERFACE_PARALLEL
static void display_board_init(void)
{
    ESP_LOGI(TAG, "Initializing parallel display (%dx%d)", CONFIG_UI_DISPLAY_WIDTH, CONFIG_UI_DISPLAY_HEIGHT);
    /* TODO: Add parallel display driver initialization here */
}
#else
static void display_board_init(void)
{
    // No display interface selected
}
#endif

void ui_init(const ui_screen_config_t *config)
{
    int hor_res = config ? config->hor_res : CONFIG_UI_DISPLAY_WIDTH;
    int ver_res = config ? config->ver_res : CONFIG_UI_DISPLAY_HEIGHT;

    lv_init();
    display_board_init();

    temp_label = lv_label_create(lv_scr_act());
    hum_label = lv_label_create(lv_scr_act());
    lv_obj_align(temp_label, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_align(hum_label, LV_ALIGN_TOP_MID, 0, 30);
    lv_label_set_text(temp_label, "Temp: --.-C");
    lv_label_set_text(hum_label, "Humidity: --.-%");
    ESP_LOGI(TAG, "UI initialized (%dx%d)", hor_res, ver_res);
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
