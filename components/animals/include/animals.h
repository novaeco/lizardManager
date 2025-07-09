#pragma once
#include "esp_err.h"
#include <stddef.h>

#define ANIMALS_MAX 32

typedef struct {
    char id[16];
    char species[32];
    char sex[8];
    char birthdate[11];
    char legal_status[32];
    char photos[128];
} animal_t;

esp_err_t animals_init(void);
esp_err_t animals_add(const animal_t *animal);
esp_err_t animals_get(const char *id, animal_t *out);
esp_err_t animals_update(const animal_t *animal);
esp_err_t animals_remove(const char *id);
esp_err_t animals_list(animal_t *out, size_t max, size_t *count);
