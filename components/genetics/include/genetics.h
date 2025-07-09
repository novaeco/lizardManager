#pragma once
#include "esp_err.h"
#include <stddef.h>
#include <stdbool.h>

#define GENETICS_MAX_SPECIES 16
#define GENETICS_MAX_MORPHS 16
#define GENETICS_MAX_LINKS 32

typedef struct {
    char name[32];
    bool recessive;
} morph_def_t;

typedef struct {
    char name[32];
    morph_def_t morphs[GENETICS_MAX_MORPHS];
    size_t morph_count;
} species_t;

typedef struct {
    char offspring[16];
    char parent1[16];
    char parent2[16];
} parent_link_t;

esp_err_t genetics_add_species(const species_t *species);
esp_err_t genetics_get_species(const char *name, species_t *out);

esp_err_t genetics_link_parents(const char *offspring, const char *parent1, const char *parent2);
esp_err_t genetics_get_parents(const char *offspring, parent_link_t *out);

esp_err_t genetics_calc_possible_traits(const morph_def_t *parent1_traits, size_t parent1_count,
                                        const morph_def_t *parent2_traits, size_t parent2_count,
                                        morph_def_t *out_traits, size_t max_traits, size_t *out_count);

