#include "i18n.h"
#include "sdkconfig.h"

static const char *en[I18N_KEY_COUNT] = {
    [I18N_TEMP_STATIC] = "Temp: --.-C",
    [I18N_HUM_STATIC] = "Humidity: --.-%",
    [I18N_LAST_FEED_STATIC] = "Last feed: --",
    [I18N_NEXT_FEED_STATIC] = "Next feed: --",
    [I18N_STATS_STATIC] = "Stats: --",
    [I18N_TEMP] = "Temp: %.1fC",
    [I18N_HUMIDITY] = "Humidity: %.1f%%",
    [I18N_LAST_FEED] = "Last feed: %s",
    [I18N_NEXT_FEED] = "Next feed: %d day(s)",
    [I18N_STATS] = "Stats: %d feedings, %.1f d avg"
};

static const char *fr[I18N_KEY_COUNT] = {
    [I18N_TEMP_STATIC] = "Temp\xC3\xA9rature: --.-C",
    [I18N_HUM_STATIC] = "Humidit\xC3\xA9: --.-%",
    [I18N_LAST_FEED_STATIC] = "Dernier repas: --",
    [I18N_NEXT_FEED_STATIC] = "Prochain repas: --",
    [I18N_STATS_STATIC] = "Stats: --",
    [I18N_TEMP] = "Temp\xC3\xA9rature: %.1fC",
    [I18N_HUMIDITY] = "Humidit\xC3\xA9: %.1f%%",
    [I18N_LAST_FEED] = "Dernier repas: %s",
    [I18N_NEXT_FEED] = "Prochain repas: %d jour(s)",
    [I18N_STATS] = "Stats: %d repas, moyenne %.1f j"
};

static const char *de[I18N_KEY_COUNT] = {
    [I18N_TEMP_STATIC] = "Temp.: --.-C",
    [I18N_HUM_STATIC] = "Feuchtigkeit: --.-%",
    [I18N_LAST_FEED_STATIC] = "Letzte F\xC3\xBCtterung: --",
    [I18N_NEXT_FEED_STATIC] = "N\xC3\xA4chste F\xC3\xBCtterung: --",
    [I18N_STATS_STATIC] = "Statistik: --",
    [I18N_TEMP] = "Temp.: %.1fC",
    [I18N_HUMIDITY] = "Feuchtigkeit: %.1f%%",
    [I18N_LAST_FEED] = "Letzte F\xC3\xBCtterung: %s",
    [I18N_NEXT_FEED] = "N\xC3\xA4chste F\xC3\xBCtterung: %d Tag(e)",
    [I18N_STATS] = "Statistik: %d F\xC3\xBCtterungen, %.1f T Schnitt"
};

static const char *es[I18N_KEY_COUNT] = {
    [I18N_TEMP_STATIC] = "Temp.: --.-C",
    [I18N_HUM_STATIC] = "Humedad: --.-%",
    [I18N_LAST_FEED_STATIC] = "\xC3\x9Altima comida: --",
    [I18N_NEXT_FEED_STATIC] = "Pr\xC3\xB3xima comida: --",
    [I18N_STATS_STATIC] = "Estad\xC3\xADsticas: --",
    [I18N_TEMP] = "Temp.: %.1fC",
    [I18N_HUMIDITY] = "Humedad: %.1f%%",
    [I18N_LAST_FEED] = "\xC3\x9Altima comida: %s",
    [I18N_NEXT_FEED] = "Pr\xC3\xB3xima comida: %d d\xC3\xADa(s)",
    [I18N_STATS] = "Estad\xC3\xADsticas: %d comidas, %.1f d prom"
};

static const char **current = en;

void i18n_init(void)
{
#if CONFIG_I18N_LANG_FR
    current = fr;
#elif CONFIG_I18N_LANG_DE
    current = de;
#elif CONFIG_I18N_LANG_ES
    current = es;
#else
    current = en;
#endif
}

const char *i18n_str(i18n_key_t key)
{
    if (key >= I18N_KEY_COUNT) return "";
    return current[key];
}
