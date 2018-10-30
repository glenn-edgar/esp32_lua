#ifndef _BLE_CUSTOM_BEACON_H_
#define _BLE_CUSTOM_BEACON_H_

void ibeacon_custom_setup(void);
void ibeacon_custom_stop(void);
void ibeacon_custom_start(int broadcast_interval_msec, uint16_t major, uint16_t minor);
void ibeacon_custom_change_numbers(uint16_t major, uint16_t minor);



#endif