#ifndef _MQTT_CTRL_LOAD_SETUP_DATA_H_
#define _MQTT_CTRL_LOAD_SETUP_DATA_H_



void mqtt_ctl_load_configuration(esp_mqtt_client_config_t *mqtt_cfg,char *base_topic, uint32_t *topic_size );


#endif