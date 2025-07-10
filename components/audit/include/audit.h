#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t audit_init(void);
esp_err_t audit_log(const char *username, const char *action);
void audit_close(void);

#ifdef __cplusplus
}
#endif
