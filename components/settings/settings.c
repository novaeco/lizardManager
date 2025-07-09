#include "settings.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "settings";
static nvs_handle_t handle;

// Default values via menuconfig
#ifndef CONFIG_SETTINGS_TEMP_MIN
#define CONFIG_SETTINGS_TEMP_MIN 20.0
#endif
#ifndef CONFIG_SETTINGS_TEMP_MAX
#define CONFIG_SETTINGS_TEMP_MAX 30.0
#endif
#ifndef CONFIG_SETTINGS_HUM_MIN
#define CONFIG_SETTINGS_HUM_MIN 40.0
#endif
#ifndef CONFIG_SETTINGS_HUM_MAX
#define CONFIG_SETTINGS_HUM_MAX 60.0
#endif

esp_err_t settings_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "NVS init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    ret = nvs_open("cfg", NVS_READWRITE, &handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "NVS open failed: %s", esp_err_to_name(ret));
        return ret;
    }
    return ESP_OK;
}

static esp_err_t get_value(const char *key, float def_val, float *out)
{
    int32_t val = (int32_t)(def_val * 10);
    nvs_get_i32(handle, key, &val);
    if (out) *out = val / 10.0f;
    return ESP_OK;
}

static esp_err_t set_value(const char *key, float val)
{
    int32_t iv = (int32_t)(val * 10);
    esp_err_t ret = nvs_set_i32(handle, key, iv);
    if (ret == ESP_OK) nvs_commit(handle);
    return ret;
}

esp_err_t settings_get_temp_range(float *min_c, float *max_c)
{
    get_value("tmin", CONFIG_SETTINGS_TEMP_MIN, min_c);
    get_value("tmax", CONFIG_SETTINGS_TEMP_MAX, max_c);
    return ESP_OK;
}

esp_err_t settings_set_temp_range(float min_c, float max_c)
{
    set_value("tmin", min_c);
    set_value("tmax", max_c);
    return ESP_OK;
}

esp_err_t settings_get_hum_range(float *min_p, float *max_p)
{
    get_value("hmin", CONFIG_SETTINGS_HUM_MIN, min_p);
    get_value("hmax", CONFIG_SETTINGS_HUM_MAX, max_p);
    return ESP_OK;
}

esp_err_t settings_set_hum_range(float min_p, float max_p)
{
    set_value("hmin", min_p);
    set_value("hmax", max_p);
    return ESP_OK;
}
