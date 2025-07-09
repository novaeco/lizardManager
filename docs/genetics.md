# Genetics Module

The genetics component stores species and morph definitions for basic trait calculations.
It allows linking parents to their offspring and provides a helper to estimate
possible traits that may appear based on simple dominance rules.

## Usage

1. Include the header in your component or application:

```c
#include "genetics.h"
```

2. Define species along with their available morphs. Recessive traits should have
   `recessive` set to `true`.

```c
species_t leopard_gecko = {
    .name = "Eublepharis macularius",
    .morphs = {
        { .name = "Albino", .recessive = true },
        { .name = "High Yellow", .recessive = false }
    },
    .morph_count = 2
};

genetics_add_species(&leopard_gecko);
```

3. Link parents to an offspring using their identifiers. These identifiers could
   be animal IDs stored by the `animals` component or any unique string.

```c
// store that "baby1" is the offspring of "mom1" and "dad1"
genetics_link_parents("baby1", "mom1", "dad1");
```

4. Calculate possible traits for an offspring by providing the parents' morphs.
   Dominant traits are passed on if present in either parent. Recessive traits
   require both parents to carry the trait.

```c
morph_def_t mother_traits[] = {
    { .name = "Albino", .recessive = true }
};

morph_def_t father_traits[] = {
    { .name = "High Yellow", .recessive = false },
    { .name = "Albino", .recessive = true }
};

morph_def_t possible[GENETICS_MAX_MORPHS];
size_t possible_count = 0;

if (genetics_calc_possible_traits(mother_traits, 1,
                                 father_traits, 2,
                                 possible, GENETICS_MAX_MORPHS,
                                 &possible_count) == ESP_OK) {
    // possible now holds the traits that offspring may display
}
```

The module does not persist data; if required, call the functions during
initialization to rebuild the species list and parent links.

