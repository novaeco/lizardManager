#include "esp_spiffs.h"
#include "esp_err.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

static char temp_dir[] = "/tmp/spiffsXXXXXX";

esp_err_t esp_vfs_spiffs_register(const esp_vfs_spiffs_conf_t *conf)
{
    (void)conf;
    if (!mkdtemp(temp_dir)) return ESP_FAIL;
    mkdir("/spiffs", 0755);
    symlink(temp_dir, "/spiffs");
    return ESP_OK;
}

void esp_vfs_spiffs_unregister(const char *label)
{
    (void)label;
    unlink("/spiffs");
    rmdir(temp_dir);
}
