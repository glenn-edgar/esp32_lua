

#ifndef _SD_CARD_H_
#define _SD_CARD_H_

#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"

char *sd_get_prefix(void);
bool initialize_sd_card(void);
void terminate_file_system(void);
void test_file_system(void);
bool is_sd_card_mounted(void );
#endif