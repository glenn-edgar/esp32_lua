
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_types.h"
#include "watchdog.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#include "driver/gpio.h"
#include "mqtt_ctrl.h"
#include "app_analog_mqtt_update.h"
#include "app_analog_analog_configuration.h"
#include "app_analog_event_queue.h"


#include "app_analog_task.h"


#define SAMPLE_NUMBER 5
static xTaskHandle current_task_handle;
static xQueueHandle mqtt_queue;

static APP_MQTT_STRUCTURE mqtt_data;
static uint32_t ad1_channel_number;
static AD1_CONTROL_T *ad1_control;

static float ma_r = .9;
static float ma_dc_avg[ADC1_CHANNEL_MAX];
static float ma_dc_sd[ADC1_CHANNEL_MAX];


static void analog_sampling_task(void *arg);
static void initialize_moving_average_structures(void);
static void pack_mqtt_data(void);
static void initialize_moving_averages( uint32_t channel );
static void compute_moving_averages( uint32_t channel );
static void start_timer_interrupt(void);






void app_analog_initialize_sampling_task(  xQueueHandle mqtt_que )
{
    
    mqtt_queue = mqtt_que;
    ad1_channel_number = app_analog_get_analog_data( &ad1_control);
    xTaskCreate(analog_sampling_task, "analog_task", 4000, NULL, 40, NULL);
    
}









static void analog_sampling_task(void *arg)
{
    
    uint32_t ad_buffer_count;
    static uint32_t mqtt_update_time;
    uint32_t notify_value;
    BaseType_t xResult;
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS( 1000 );
    current_task_handle =  xTaskGetCurrentTaskHandle();
    
    mqtt_update_time = 100*15;  // sample rate is .01 sec mqtt update rate is 15 seconds
    
  
    ad_buffer_count = 0;
    initialize_moving_average_structures();
    for(int i = 0; i< ad1_channel_number; i++)
    {            
        initialize_moving_averages( i );
    }    
    task_wait_for_mqtt_connection();
    wdt_task_subscribe();
    start_timer_interrupt();
    while(1) 
    {
       
       wdt_reset_task_time();
       /* Wait to be notified of an interrupt. */
        xResult = xTaskNotifyWait( ULONG_MAX,    /* Clear all bits on exit. */
                           ULONG_MAX,        /* Clear all bits on exit. */
                           &notify_value, /* Stores the notified value. */
                           xMaxBlockTime );   
        if( xResult == 0){ continue; }                   
        for(int i = 0; i< ad1_channel_number; i++)
        {            
        compute_moving_averages( i );
        }
  
  
        ad_buffer_count++;
        if( ad_buffer_count >= mqtt_update_time)
        {
           ad_buffer_count = 0;
               
           pack_mqtt_data();
           xQueueSend(mqtt_queue, &mqtt_data, 0);
                  
        }

 

    }

}

static void initialize_moving_average_structures(void)
{
   memset(ma_dc_avg,0,sizeof(ma_dc_avg));
   memset(ma_dc_sd,0,sizeof(ma_dc_sd));

    
}

static void initialize_moving_averages( uint32_t channel )
{
    uint32_t avg_sample;
    float voltage;
    uint32_t adc_channel;
    avg_sample = 0;
    
    adc_channel = ad1_control[channel].adc_channel;
    for(int i = 0; i < SAMPLE_NUMBER; i++)
    {
        avg_sample += adc1_get_raw(adc_channel);
    }
    avg_sample = avg_sample/SAMPLE_NUMBER;
    voltage = (esp_adc_cal_raw_to_voltage(avg_sample, ad1_control[channel].adc_chars))/1000.;
    ma_dc_avg[channel] = voltage;
    ma_dc_sd[channel] = 0;
}

static void compute_moving_averages( uint32_t channel )
{
    uint32_t avg_sample;
    float voltage;
    uint32_t adc_channel;
    avg_sample = 0;
    
    adc_channel = ad1_control[channel].adc_channel;
    for(int i = 0; i < SAMPLE_NUMBER; i++)
    {
        avg_sample += adc1_get_raw(adc_channel);
    }
    avg_sample = avg_sample/SAMPLE_NUMBER;
    voltage = (esp_adc_cal_raw_to_voltage(avg_sample, ad1_control[channel].adc_chars))/1000.;
    ma_dc_avg[channel] = ma_r*ma_dc_avg[channel] +(1.-ma_r)*voltage;
    ma_dc_sd[channel] = ma_r*ma_dc_sd[channel] +
                        (1.-ma_r)*(ma_dc_avg[channel] - voltage)*(ma_dc_avg[channel] - voltage);
}

static void pack_mqtt_data(void)
{
    memset(&mqtt_data,0,sizeof(mqtt_data));
    mqtt_data.channel_number = ad1_channel_number;
    for(int i = 0; i< ad1_channel_number;i++)
    {
        mqtt_data.app_analog_mqtt_channel[i].ad_channel = ad1_control[i].adc_channel;
        mqtt_data.app_analog_mqtt_channel[i].dc_average = ma_dc_avg[i];
        mqtt_data.app_analog_mqtt_channel[i].sd_average = ma_dc_sd[i];
        
        
    }

}

/*
  Code taken from https://github.com/sankarcheppali/esp_idf_esp32_posts/blob/master/timer_group/main/timer_group.c
  Thanks for the help
*/

#define TIMER_INTR_SEL TIMER_INTR_LEVEL  /*!< Timer level interrupt */
#define TIMER_GROUP    TIMER_GROUP_0     /*!< Test on timer group 0 */
#define TIMER_DIVIDER   80               /*!< Hardware timer clock divider, 80 to get 1MHz clock to timer */
#define TIMER_SCALE    (TIMER_BASE_CLK / TIMER_DIVIDER)  /*!< used to calculate counter value */
#define TIMER_FINE_ADJ   (0*(TIMER_BASE_CLK / TIMER_DIVIDER)/1000000) /*!< used to compensate alarm value */
#define TIMER_INTERVAL0_SEC   (0.01)   /*!< test interval for timer 0 */




void IRAM_ATTR timer_group0_isr(void *para)
{// timer group 0, ISR
    int timer_idx = (int) para;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     uint32_t intr_status = TIMERG0.int_st_timers.val;
      if((intr_status & BIT(timer_idx)) && timer_idx == TIMER_0) {
          TIMERG0.hw_timer[timer_idx].update = 1;
          TIMERG0.int_clr_timers.t0 = 1;
          TIMERG0.hw_timer[timer_idx].config.alarm_en = 1;

         
          xTaskNotifyFromISR( current_task_handle,
                       1,
                       eSetBits,
                       &xHigherPriorityTaskWoken );
          portYIELD_FROM_ISR(  );
          
      }
}

static void start_timer_interrupt(void)
{
    int timer_group = TIMER_GROUP_0;
    int timer_idx = TIMER_0;
    timer_config_t config;
    config.alarm_en = 1;
    config.auto_reload = 1;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = TIMER_DIVIDER;
    config.intr_type = TIMER_INTR_SEL;
    config.counter_en = TIMER_PAUSE;
    /*Configure timer*/
    timer_init(timer_group, timer_idx, &config);
    /*Stop timer counter*/
    timer_pause(timer_group, timer_idx);
    /*Load counter value */
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
    /*Set alarm value*/
    timer_set_alarm_value(timer_group, timer_idx, (TIMER_INTERVAL0_SEC * TIMER_SCALE) - TIMER_FINE_ADJ);
    /*Enable timer interrupt*/
    timer_enable_intr(timer_group, timer_idx);
    /*Set ISR handler*/
    timer_isr_register(timer_group, timer_idx, timer_group0_isr, (void*) timer_idx, ESP_INTR_FLAG_IRAM, NULL);
    /*Start timer counter*/
    gpio_set_direction(GPIO_NUM_16,GPIO_MODE_OUTPUT);
    timer_start(timer_group, timer_idx);
}