
#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

void initialize_watchdog(int time_out_seconds, bool panic);
void wdt_task_subscribe(void);
void wdt_task_unsubscribe(void);
void wd_task_status(void);
void wdt_reset_task_time(void);

#endif