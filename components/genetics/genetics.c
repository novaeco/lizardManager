#include "genetics.h"
#include <string.h>

static species_t species_list[GENETICS_MAX_SPECIES];
static size_t species_count = 0;

static parent_link_t links[GENETICS_MAX_LINKS];
static size_t link_count = 0;

static int find_species(const char *name)
{
    for (size_t i = 0; i < species_count; ++i) {
        if (strcmp(species_list[i].name, name) == 0) return (int)i;
    }
    return -1;
}

esp_err_t genetics_add_species(const species_t *species)
{
    if (!species || species_count >= GENETICS_MAX_SPECIES) return ESP_ERR_INVALID_ARG;
    if (find_species(species->name) >= 0) return ESP_ERR_INVALID_ARG;
    species_list[species_count++] = *species;
    return ESP_OK;
}

esp_err_t genetics_get_species(const char *name, species_t *out)
{
    if (!name || !out) return ESP_ERR_INVALID_ARG;
    int idx = find_species(name);
    if (idx < 0) return ESP_ERR_NOT_FOUND;
    *out = species_list[idx];
    return ESP_OK;
}

esp_err_t genetics_link_parents(const char *offspring, const char *parent1, const char *parent2)
{
    if (!offspring || !parent1 || !parent2 || link_count >= GENETICS_MAX_LINKS) {
        return ESP_ERR_INVALID_ARG;
    }
    strncpy(links[link_count].offspring, offspring, sizeof(links[link_count].offspring) - 1);
    strncpy(links[link_count].parent1, parent1, sizeof(links[link_count].parent1) - 1);
    strncpy(links[link_count].parent2, parent2, sizeof(links[link_count].parent2) - 1);
    link_count++;
    return ESP_OK;
}

esp_err_t genetics_get_parents(const char *offspring, parent_link_t *out)
{
    if (!offspring || !out) return ESP_ERR_INVALID_ARG;
    for (size_t i = 0; i < link_count; ++i) {
        if (strcmp(links[i].offspring, offspring) == 0) {
            *out = links[i];
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

static bool has_trait(const morph_def_t *list, size_t count, const char *name)
{
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(list[i].name, name) == 0) return true;
    }
    return false;
}

esp_err_t genetics_calc_possible_traits(const morph_def_t *parent1_traits, size_t parent1_count,
                                        const morph_def_t *parent2_traits, size_t parent2_count,
                                        morph_def_t *out_traits, size_t max_traits, size_t *out_count)
{
    if (!parent1_traits || !parent2_traits || !out_traits || !out_count) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t idx = 0;
    for (size_t i = 0; i < parent1_count; ++i) {
        bool present_p2 = has_trait(parent2_traits, parent2_count, parent1_traits[i].name);
        bool include = !parent1_traits[i].recessive || present_p2;
        if (include && !has_trait(out_traits, idx, parent1_traits[i].name)) {
            if (idx >= max_traits) return ESP_ERR_NO_MEM;
            out_traits[idx++] = parent1_traits[i];
        }
    }
    for (size_t i = 0; i < parent2_count; ++i) {
        if (has_trait(out_traits, idx, parent2_traits[i].name)) continue;
        bool present_p1 = has_trait(parent1_traits, parent1_count, parent2_traits[i].name);
        bool include = !parent2_traits[i].recessive || present_p1;
        if (include) {
            if (idx >= max_traits) return ESP_ERR_NO_MEM;
            out_traits[idx++] = parent2_traits[i];
        }
    }
    *out_count = idx;
    return ESP_OK;
}

