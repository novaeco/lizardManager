#include "scanner.h"
#include "esp_log.h"

static const char *TAG = "scanner";
static scanner_cb_t scan_cb = NULL;

esp_err_t scanner_init(scanner_cb_t cb)
{
#if CONFIG_SCANNER_ENABLE
    scan_cb = cb;
    ESP_LOGI(TAG, "Scanner initialized (stub)");
    // TODO: add RFID or QR-code hardware integration
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}
