#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "driver/pcnt.h"
#include "mqtt_client.h"
#include "mqtt_ctrl.h"
#include "mqtt_ctrl_subscription.h"
#include "driver/mcpwm.h"
#include "msgpack_rx_handler.h"
#include "msg_dict_stream.h"
#include "watchdog.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#include "app_pulse_setup.h"


                             

typedef struct
{
    uint32_t gpio_pin;
    uint32_t pulse_counts;
  
}APP_PULSE_TIMER_CONTROL;


static uint32_t pulse_update_rate;
static uint32_t pulse_counter_number;
static uint32_t pulse_filter_count;
static APP_PULSE_TIMER_CONTROL app_pulse_counters[PCNT_UNIT_MAX] ;

static bool app_pulse_read_file_configuration( void );  
static bool app_pulse_find_set_configuration(   uint32_t size, char *buffer);
static void send_mqtt_message(uint32_t counter_number);




static void counter_task(void *arg);

static inline void setup_a_pulse_counter( uint32_t counter, uint32_t gpio_pin, uint32_t min_pulse_length )
{
    pcnt_config_t pcnt_config;
    pcnt_config.pulse_gpio_num = gpio_pin;
    pcnt_config.ctrl_gpio_num = -1;
    pcnt_config.unit = counter;
    pcnt_config.channel = PCNT_CHANNEL_0,
    pcnt_config.lctrl_mode = PCNT_MODE_KEEP, // Reverse counting direction if low
    pcnt_config.hctrl_mode = PCNT_MODE_KEEP,    // Keep the primary counter mode if high

    pcnt_config.pos_mode = PCNT_COUNT_INC;   // Count up on the positive edge
    pcnt_config.neg_mode = PCNT_COUNT_INC;   // Keep the counter value on the negative edgePCNT_COUNT_DIS
    pcnt_config.counter_h_lim = 0x7fff;
    pcnt_config.counter_l_lim = 0;


    /* Initialize PCNT unit */
    
    pcnt_unit_config(&pcnt_config);
    
    pcnt_set_filter_value(counter, min_pulse_length);
    
    pcnt_counter_clear(counter);
    pcnt_counter_resume(counter);


}

static inline void initialize_pulse_accumulators(uint32_t pulse_counter_number)
{
    for(int i = 0; i < pulse_counter_number;i++)
    {
       app_pulse_counters[i].pulse_counts = 0;
        
    }
} 

static inline int32_t read_a_counter( uint32_t counter)
{
    int16_t count;
    
    pcnt_counter_pause(counter);
    pcnt_get_counter_value(counter, &count);
    pcnt_counter_clear(counter);
    pcnt_counter_resume(counter);
    return count;
    
}

 

static inline void setup_pulse_counters( uint32_t number_of_counters)
{
    for(int i = 0; i< number_of_counters;i++)
    {
       setup_a_pulse_counter( i,app_pulse_counters[i].gpio_pin,pulse_filter_count );
        
        
    }
    
    
    
}


static inline void read_counter_values(uint32_t number_of_counters)
{
    for(int i = 0; i< number_of_counters;i++)
    {
       app_pulse_counters[i].pulse_counts +=   read_a_counter( i);
        
        
    }
    
    
    
}

                             
void initialize_app_pulse_main(void)
{

    pulse_counter_number = 0;
    memset(app_pulse_counters,0,sizeof(app_pulse_counters));
    
    if( app_pulse_read_file_configuration( ) == true)
    {

       
       xTaskCreate(counter_task, "counter_task", 4000, NULL, 40, NULL);
    }

        
}
    










static void counter_task(void *arg)
{
    uint32_t pulse_count;
    

    
    pulse_count = 0;
    pulse_update_rate = 15; // fifteen seconds
    setup_pulse_counters( pulse_counter_number);
    initialize_pulse_accumulators(pulse_counter_number);
    task_wait_for_mqtt_connection();
    
    wdt_task_subscribe();
    while(1)
    {
       wdt_reset_task_time();
       vTaskDelay(1000 / portTICK_PERIOD_MS);
        
       read_counter_values(pulse_counter_number);
       pulse_count +=1;
       if(pulse_count >= pulse_update_rate) 
       {
           send_mqtt_message(pulse_counter_number);
           pulse_count = 0;
           initialize_pulse_accumulators(pulse_counter_number);
       }
           
    
    }
       
    
}

   
/*
**  MESSAGE PACK  input STUFF
**
*/    
    
static void send_mqtt_message(uint32_t counter_number)
{
   MSG_PACK_ELEMENT *msg_pack;
   uint32_t  pack_index= 0;
   uint32_t  msg_pack_number;
   int       pack_buffer_size;
   char      *pack_buffer;
   
   msg_pack_number = 3*counter_number+2;
   msg_pack_number +=4; //safety
   
   msg_pack = malloc(msg_pack_number*sizeof(MSG_PACK_ELEMENT));
   
   msg_dict_pack_string(&msg_pack[pack_index++],"TOPIC","INPUT/PULSE_COUNT/VALUE"); 
   
   
   msg_dict_pack_array(&msg_pack[pack_index++],"DATA",counter_number);
   for(  int i = 0;i<counter_number;i++)
   {
      msg_dict_pack_map(&msg_pack[pack_index++],NULL, 2);
      msg_dict_pack_unsigned_integer(&msg_pack[pack_index++],"GPIO_PIN", app_pulse_counters[i].gpio_pin);
      msg_dict_pack_unsigned_integer(&msg_pack[pack_index++],"COUNTS", app_pulse_counters[i].pulse_counts);
       
       
   }
   
 
   pack_buffer = msg_dict_stream( &pack_buffer_size,pack_index,msg_pack);
   
   mqtt_clt_publish("INPUT/PULSE_COUNT/VALUE", pack_buffer,pack_buffer_size );
   free(pack_buffer);
   free(msg_pack);
 
    
    
}      










/*
**  MESSAGE PACK  input STUFF
**
*/
static bool app_pulse_read_file_configuration( void ) 
{
    bool          return_value;
    cmp_ctx_t ctx;
    char     *buffer;
    uint32_t buffer_size = 1000;
    
       
    
    if( msgpack_rx_handler_file(&ctx,"/spiffs/IO_PULSE.MPK" ,  &buffer,&buffer_size ) != true)
    {
         
         return false;
    }
   

    return_value = app_pulse_find_set_configuration(   buffer_size, buffer);
   
    free(buffer);
    return return_value;    
    
}

static bool app_pulse_find_set_configuration(   uint32_t size, char *buffer)
{
   cmp_ctx_t ctx;
   uint32_t array_count;
   uint32_t gpio_array[PCNT_UNIT_MAX];
    
   msgpack_rx_handler_init(&ctx, buffer, size);
   
   if( msgpack_rx_handler_find_unsigned(&ctx,"COUNTER_UPDATE_RATE", &pulse_update_rate) != true )
   {
       
       return false;
        
   } 
   if( msgpack_rx_handler_find_unsigned(&ctx,"COUNTER_FILTER_COUNT", &pulse_filter_count ) != true )
   {
       
       return false;
        
   }   
   array_count = PCNT_UNIT_MAX;
   if(msgpack_rx_handler_find_array_count(&ctx,"GPIO_PINS",&array_count) == false)
   {
       
       return false;
   }
   if( msgpack_rx_handler_find_array_unsigned(&ctx,"GPIO_PINS",&array_count, gpio_array) != true )
   {
        
       return false;
        
   } 
   pulse_counter_number = array_count;   

   
   for(int i = 0; i< pulse_counter_number;i++)
   {
       
      app_pulse_counters[i].gpio_pin = gpio_array[i];
       
   }
   return true;
}
   
