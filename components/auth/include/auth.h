#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUTH_ROLE_OWNER,
    AUTH_ROLE_CARETAKER,
    AUTH_ROLE_VET,
    AUTH_ROLE_UNKNOWN
} auth_role_t;

esp_err_t auth_init(void);
esp_err_t auth_add_user(const char *username, const char *password, auth_role_t role);
esp_err_t auth_authenticate(const char *username, const char *password, auth_role_t *role_out);

#ifdef __cplusplus
}
#endif
