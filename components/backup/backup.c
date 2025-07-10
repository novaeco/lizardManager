#include "backup.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "mbedtls/aes.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "backup";
static bool wifi_initialized = false;

static esp_err_t wifi_connect(void)
{
    if (wifi_initialized) return ESP_OK;
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        }
    };
    strncpy((char *)wifi_config.sta.ssid, CONFIG_BACKUP_WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, CONFIG_BACKUP_WIFI_PASS, sizeof(wifi_config.sta.password));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Connecting to Wi-Fi SSID %s", CONFIG_BACKUP_WIFI_SSID);
    ESP_ERROR_CHECK(esp_wifi_connect());
    wifi_initialized = true;
    return ESP_OK;
}

static esp_err_t encrypt_buffer(const uint8_t *in, size_t len, uint8_t *out)
{
    uint8_t key[16];
    size_t key_len = strlen(CONFIG_BACKUP_AES_KEY) / 2;
    for (size_t i = 0; i < key_len && i < sizeof(key); ++i) {
        sscanf(CONFIG_BACKUP_AES_KEY + i * 2, "%2hhx", &key[i]);
    }
    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key, 128);
    uint8_t iv[16] = {0};
    size_t blocks = (len + 15) / 16;
    for (size_t i = 0; i < blocks; ++i) {
        uint8_t block[16] = {0};
        size_t copy = (len - i * 16) >= 16 ? 16 : (len - i * 16);
        memcpy(block, in + i * 16, copy);
        mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, 16, iv, block, out + i * 16);
    }
    mbedtls_aes_free(&ctx);
    return ESP_OK;
}

static esp_err_t upload_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) return ESP_FAIL;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *buf = malloc(len);
    if (!buf) { fclose(f); return ESP_ERR_NO_MEM; }
    fread(buf, 1, len, f);
    fclose(f);
    uint8_t *enc = malloc(len + 16);
    if (!enc) { free(buf); return ESP_ERR_NO_MEM; }
    encrypt_buffer(buf, len, enc);
    free(buf);
    esp_http_client_config_t config = {
        .url = CONFIG_BACKUP_SERVER_URL,
        .method = HTTP_METHOD_POST,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_post_field(client, (const char *)enc, ((len + 15) / 16) * 16);
    esp_err_t ret = esp_http_client_perform(client);
    esp_http_client_cleanup(client);
    free(enc);
    return ret;
}

esp_err_t backup_init(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    return wifi_connect();
}

esp_err_t backup_manual(void)
{
    esp_err_t ret = wifi_connect();
    if (ret != ESP_OK) return ret;
    ret = upload_file("/spiffs/readings.csv");
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Logs uploaded");
    }
    return ret;
}

static void backup_task(void *arg)
{
    while (1) {
        backup_manual();
        vTaskDelay(pdMS_TO_TICKS(*(uint32_t *)arg * 3600000));
    }
}

esp_err_t backup_schedule(uint32_t hours)
{
    xTaskCreate(backup_task, "backup_task", 8192, &hours, 5, NULL);
    return ESP_OK;
}
