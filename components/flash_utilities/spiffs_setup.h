#ifndef _SPIFFS_SETUP_H_
#define _SPIFFS_SETUP_H_

char *spiffs_get_prefix(void);
bool initialize_spiffs(void);
int format_spiffs_drive(void);


#endif
