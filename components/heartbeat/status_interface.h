#ifndef _STATUS_INTERFACE_H_
#define _STATUS_INTERFACE_H_

void init_status_interface(void);
void change_status_value(uint32_t mask, uint32_t value);
uint32_t get_status_value(void);

#endif