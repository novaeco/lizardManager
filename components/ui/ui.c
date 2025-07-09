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
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf1;
#define LV_TICK_PERIOD_MS 2
static esp_timer_handle_t tick_timer;

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
static void display_board_init(void)
{
    ESP_LOGI(TAG, "Initializing SPI display (%dx%d)", CONFIG_UI_DISPLAY_WIDTH, CONFIG_UI_DISPLAY_HEIGHT);

    static esp_lcd_panel_handle_t panel_handle = NULL;

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

    buf1 = heap_caps_malloc(CONFIG_UI_DISPLAY_WIDTH * 10 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, CONFIG_UI_DISPLAY_WIDTH * 10);
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = CONFIG_UI_DISPLAY_WIDTH;
    disp_drv.ver_res = CONFIG_UI_DISPLAY_HEIGHT;
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
