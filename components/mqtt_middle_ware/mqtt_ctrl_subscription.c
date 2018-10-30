#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"


#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt_ctrl_load_setup_data.h"
#include "wifi_station_setup.h"

#include "mqtt_ctrl.h"

#include "mqtt_heartbeat.h"
#include "message_processor.h"
#include "mqtt_ctrl_subscription.h"

#define SUBSCRIPTION_MAX 32
                        
                                
typedef struct 
{
    SUBSCRITION_FUNCTION sub_function;
    char *topic;  // variable will be malloced
}SUBSCRIPTION_TABLE;

static uint32_t subscription_number;
static SUBSCRIPTION_TABLE subscription_table[SUBSCRIPTION_MAX];      
                          
static char *base_topic;                     

static bool check_for_duplicates_sub_table(char *topic );
static void process_builtin_commands (esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data );





  
void mqtt_initialize_subscriptions(char *base_top)
{
      base_topic = base_top;
      subscription_number = 0;    
      memset(subscription_table,0,sizeof(subscription_table));
      mqtt_ctrl_register_subscription("BUILT_IN_CMD", process_builtin_commands);
      initialize_mqtt_heart_beat();
    
}

void mqtt_ctrl_register_subscription(char *topic, SUBSCRITION_FUNCTION topic_function )
{
      uint32_t size;
      char *ref_topic;
      
      if( subscription_number >= SUBSCRIPTION_MAX )
      {
          abort(); // ran out of table space
      }
      if( check_for_duplicates_sub_table(topic) == true)
      {
          abort(); // all ready in table
      }
      size = strlen(base_topic)+strlen(topic)+1;
      ref_topic = malloc(size);
      strcpy(ref_topic,base_topic);
      strcat(ref_topic,topic);
      subscription_table[subscription_number].sub_function = topic_function;
      subscription_table[subscription_number].topic = ref_topic;
      mqtt_clt_subscribe(ref_topic);
      subscription_number +=1;
      
    
    
}



void mqtt_ctl_restore_subscriptions(esp_mqtt_client_handle_t mqtt_client)
{
    printf("restore subscriptions %d \n",subscription_number);
    for(int i = 0;i<subscription_number;i++ )
    {
       printf("topic is %i %s \n",i,subscription_table[i].topic);
       mqtt_clt_subscribe(subscription_table[i].topic);
        
    }
    
}

void  mqtt_ctl_handle_data(esp_mqtt_client_handle_t mqtt_client,
                           uint32_t topic_len,
                           char *topic, 
                           uint32_t data_len, 
                           char *data  )
{
    
    
    for(int i=0;i<subscription_number;i++)
    {
        //printf("%.*s %s \n",topic_len,topic,subscription_table[i].topic);
        if(strlen(subscription_table[i].topic)!= topic_len)
        {
            continue;
        }
        if( strncmp(subscription_table[i].topic,topic,topic_len) == 0)
        {
            //printf("topic match \n");
            subscription_table[i].sub_function(mqtt_client, topic_len, topic,data_len,data);
            return;
            
        }
                   
        
        
    }
    printf("no match \n");
    //abort(); // topic function not detected
    
}



/*

Static Data

*/



static bool check_for_duplicates_sub_table(char *topic )
{
  
   int  topic_len;
   
   topic_len = strlen(topic);
   for(int i=0;i<subscription_number;i++)
    {
        if(strlen(subscription_table[i].topic)!= topic_len)
        {
            
            break;
        }
        if( strncmp(subscription_table[i].topic,topic,topic_len) == 0)
        {
           return true;
            
        }
                   
        
        
    }
   return false;
}

static void process_builtin_commands (esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data )
{                              

     int msg_pack_number;
     MSG_PACK_ELEMENT *msg_pack_buffer;
     char *pack_buffer;
     int pack_buffer_size;

                       
     if( message_processor(&msg_pack_number,&msg_pack_buffer,
                           data,data_len) == true)
     {       
              
            pack_buffer = msg_dict_stream( &pack_buffer_size,msg_pack_number,msg_pack_buffer);
            mqtt_clt_publish("BUILT_IN_CMD_REPLY", pack_buffer,pack_buffer_size);
            free(pack_buffer);
 
         
     }
     else
     {
       ;// printf("no message processor match\n");
     }
     if( msg_pack_number > 0)
     {
         free(msg_pack_buffer);
     }

}   


        
        
        
   
    
    
    
