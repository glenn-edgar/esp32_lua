

#ifndef _MQTT_CTRL_SUBSCRIPTION_H_
#define _MQTT_CTRL_SUBSCRIPTION_H_


typedef void (*SUBSCRITION_FUNCTION)(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  );


void mqtt_initialize_subscriptions(char *base_top);

void mqtt_ctrl_register_subscription(char *topic, SUBSCRITION_FUNCTION function );


void mqtt_ctl_restore_subscriptions(esp_mqtt_client_handle_t mqtt_client);

void  mqtt_ctl_handle_data(esp_mqtt_client_handle_t mqtt_client,
                           uint32_t topic_len,
                           char *topic, 
                           uint32_t data_len, 
                           char *data  );

#endif