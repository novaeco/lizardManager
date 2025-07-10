#pragma once
#include "lvgl.h"
#include <time.h>

typedef struct {
    int hor_res;
    int ver_res;
} ui_screen_config_t;

void ui_init(const ui_screen_config_t *config); // pass NULL to use defaults
void ui_set_values(float temp, float humidity);
void ui_set_feeding(time_t last_feed, int days_until_next);
void ui_set_stats(int feed_count, float avg_interval_days);
