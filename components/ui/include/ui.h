#pragma once
#include "lvgl.h"

typedef struct {
    int hor_res;
    int ver_res;
} ui_screen_config_t;

void ui_init(const ui_screen_config_t *config); // pass NULL to use defaults
void ui_set_values(float temp, float humidity);
