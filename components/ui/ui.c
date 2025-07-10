#include "ui.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include <stdio.h>
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "driver/spi_master.h"
#include "esp_heap_caps.h"

static const char *TAG = "ui";
static lv_obj_t *temp_label;
static lv_obj_t *hum_label;
static lv_obj_t *feed_label;
static lv_obj_t *reminder_label;
static lv_obj_t *stats_label;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf1;
#define LV_TICK_PERIOD_MS 2
static esp_timer_handle_t tick_timer;
static esp_lcd_panel_handle_t panel_handle;

static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    esp_lcd_panel_handle_t handle = *(esp_lcd_panel_handle_t *)drv->user_data;
    esp_lcd_panel_draw_bitmap(handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_p);
    lv_disp_flush_ready(drv);
}

static void lv_tick_task(void *arg)
{
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

static void lvgl_task(void *arg)
{
    while (true) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

#if CONFIG_UI_DISPLAY_INTERFACE_SPI
static esp_lcd_panel_handle_t display_board_init(void)
{
    ESP_LOGI(TAG, "Initializing SPI display (%dx%d)", CONFIG_UI_DISPLAY_WIDTH, CONFIG_UI_DISPLAY_HEIGHT);


    spi_bus_config_t buscfg = {
        .mosi_io_num = 23,
        .miso_io_num = -1,
        .sclk_io_num = 18,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = CONFIG_UI_DISPLAY_WIDTH * CONFIG_UI_DISPLAY_HEIGHT * 2 + 8,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = 5,
        .dc_gpio_num = 16,
        .pclk_hz = 40 * 1000 * 1000,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = 17,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    return panel_handle;
}
#elif CONFIG_UI_DISPLAY_INTERFACE_PARALLEL
static esp_lcd_panel_handle_t display_board_init(void)
{
    ESP_LOGI(TAG, "Initializing parallel display (%dx%d)", CONFIG_UI_DISPLAY_WIDTH, CONFIG_UI_DISPLAY_HEIGHT);

    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = 16,
        .wr_gpio_num = 21,
        .data_gpio_nums = {12, 13, 14, 27, 26, 25, 33, 32},
        .bus_width = 8,
        .max_transfer_bytes = CONFIG_UI_DISPLAY_WIDTH * CONFIG_UI_DISPLAY_HEIGHT * 2 + 8,
    };
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = 15,
        .pclk_hz = 10 * 1000 * 1000,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = 17,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    return panel_handle;
}
#else
static esp_lcd_panel_handle_t display_board_init(void)
{
    // No display interface selected
    return NULL;
}
#endif

void ui_init(const ui_screen_config_t *config)
{
    int hor_res = config ? config->hor_res : CONFIG_UI_DISPLAY_WIDTH;
    int ver_res = config ? config->ver_res : CONFIG_UI_DISPLAY_HEIGHT;

    lv_init();
    panel_handle = display_board_init();

    buf1 = heap_caps_malloc(hor_res * 10 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, hor_res * 10);
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = hor_res;
    disp_drv.ver_res = ver_res;
    disp_drv.user_data = &panel_handle;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    const esp_timer_create_args_t tick_args = {
        .callback = lv_tick_task,
        .name = "lv_tick"
    };
    ESP_ERROR_CHECK(esp_timer_create(&tick_args, &tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(tick_timer, LV_TICK_PERIOD_MS * 1000));

    xTaskCreate(lvgl_task, "lvgl", 4096, NULL, 5, NULL);

    temp_label = lv_label_create(lv_scr_act());
    hum_label = lv_label_create(lv_scr_act());
    feed_label = lv_label_create(lv_scr_act());
    reminder_label = lv_label_create(lv_scr_act());
    stats_label = lv_label_create(lv_scr_act());
    lv_obj_align(temp_label, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_align(hum_label, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_align(feed_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_align(reminder_label, LV_ALIGN_TOP_MID, 0, 70);
    lv_obj_align(stats_label, LV_ALIGN_TOP_MID, 0, 90);
    lv_label_set_text(temp_label, "Temp: --.-C");
    lv_label_set_text(hum_label, "Humidity: --.-%");
    lv_label_set_text(feed_label, "Last feed: --");
    lv_label_set_text(reminder_label, "Next feed: --");
    lv_label_set_text(stats_label, "Stats: --");
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

void ui_set_feeding(time_t last_feed, int days_until_next)
{
    if (!feed_label || !reminder_label) return;
    char buf[64];
    if (last_feed > 0) {
        struct tm *tm = localtime(&last_feed);
        strftime(buf, sizeof(buf), "Last feed: %Y-%m-%d", tm);
    } else {
        snprintf(buf, sizeof(buf), "Last feed: --");
    }
    lv_label_set_text(feed_label, buf);
    snprintf(buf, sizeof(buf), "Next feed: %d day(s)", days_until_next);
    lv_label_set_text(reminder_label, buf);
    lv_timer_handler();
}

void ui_set_stats(int feed_count, float avg_interval_days)
{
    if (!stats_label) return;
    char buf[64];
    snprintf(buf, sizeof(buf), "Stats: %d feedings, %.1f d avg", feed_count, avg_interval_days);
    lv_label_set_text(stats_label, buf);
    lv_timer_handler();
}
