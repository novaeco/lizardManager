#include "auth.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include "mbedtls/sha256.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define AUTH_MAX_USERS 8

static const char *TAG = "auth";

typedef struct {
    char username[32];
    char pass_hash[65];
    auth_role_t role;
} user_t;

static user_t users[AUTH_MAX_USERS];
static size_t user_count = 0;
static bool initialized = false;
static FILE *user_file = NULL;

static const char *role_to_str(auth_role_t role)
{
    switch (role) {
    case AUTH_ROLE_OWNER: return "owner";
    case AUTH_ROLE_CARETAKER: return "caretaker";
    case AUTH_ROLE_VET: return "vet";
    default: return "unknown";
    }
}

static auth_role_t str_to_role(const char *str)
{
    if (strcmp(str, "owner") == 0) return AUTH_ROLE_OWNER;
    if (strcmp(str, "caretaker") == 0) return AUTH_ROLE_CARETAKER;
    if (strcmp(str, "vet") == 0) return AUTH_ROLE_VET;
    return AUTH_ROLE_UNKNOWN;
}

static void sha256_hex(const char *data, char out[65])
{
    unsigned char hash[32];
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
    mbedtls_sha256_update(&ctx, (const unsigned char *)data, strlen(data));
    mbedtls_sha256_finish(&ctx, hash);
    mbedtls_sha256_free(&ctx);
    for (int i = 0; i < 32; ++i) sprintf(out + i * 2, "%02x", hash[i]);
    out[64] = '\0';
}

static esp_err_t save_file(void)
{
    if (!user_file) {
        user_file = fopen("/spiffs/users.csv", "w");
    } else {
        freopen("/spiffs/users.csv", "w", user_file);
    }
    if (!user_file) {
        ESP_LOGE(TAG, "Failed to open users.csv for write");
        return ESP_FAIL;
    }
    for (size_t i = 0; i < user_count; ++i) {
        fprintf(user_file, "%s,%s,%s\n", users[i].username, users[i].pass_hash,
                role_to_str(users[i].role));
    }
    fflush(user_file);
    return ESP_OK;
}

static esp_err_t load_file(void)
{
    user_file = fopen("/spiffs/users.csv", "a+");
    if (!user_file) {
        ESP_LOGE(TAG, "Failed to open users.csv");
        return ESP_FAIL;
    }
    rewind(user_file);
    char line[128];
    while (fgets(line, sizeof(line), user_file) && user_count < AUTH_MAX_USERS) {
        char *saveptr = NULL;
        char *u = strtok_r(line, ",\n", &saveptr);
        char *p = strtok_r(NULL, ",\n", &saveptr);
        char *r = strtok_r(NULL, "\n", &saveptr);
        if (!u || !p || !r) continue;
        strncpy(users[user_count].username, u, sizeof(users[user_count].username)-1);
        strncpy(users[user_count].pass_hash, p, sizeof(users[user_count].pass_hash)-1);
        users[user_count].role = str_to_role(r);
        user_count++;
    }
    return ESP_OK;
}

esp_err_t auth_init(void)
{
    if (initialized) return ESP_OK;
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "SPIFFS mount failed: %s", esp_err_to_name(ret));
        return ret;
    }
    ret = load_file();
    if (ret != ESP_OK) return ret;
    initialized = true;
    return ESP_OK;
}

esp_err_t auth_add_user(const char *username, const char *password, auth_role_t role)
{
    if (!initialized || !username || !password) return ESP_ERR_INVALID_STATE;
    if (user_count >= AUTH_MAX_USERS) return ESP_ERR_NO_MEM;
    for (size_t i = 0; i < user_count; ++i) {
        if (strcmp(users[i].username, username) == 0) return ESP_ERR_INVALID_ARG;
    }
    char hash[65];
    sha256_hex(password, hash);
    strncpy(users[user_count].username, username, sizeof(users[user_count].username)-1);
    strncpy(users[user_count].pass_hash, hash, sizeof(users[user_count].pass_hash)-1);
    users[user_count].role = role;
    user_count++;
    return save_file();
}

esp_err_t auth_authenticate(const char *username, const char *password, auth_role_t *role_out)
{
    if (!initialized || !username || !password) return ESP_ERR_INVALID_STATE;
    char hash[65];
    sha256_hex(password, hash);
    for (size_t i = 0; i < user_count; ++i) {
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].pass_hash, hash) == 0) {
            if (role_out) *role_out = users[i].role;
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}
