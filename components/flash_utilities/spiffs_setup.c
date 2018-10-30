
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"

#include "spiffs_setup.h"

char *spiffs_get_prefix(void)
{
    
  return "/spiffs/";  
    
}

bool initialize_spiffs(void)
{
    bool return_value;
    size_t total = 0, used = 0;   
    esp_err_t ret;
    
    printf("make it here %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };
    
    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            printf( "Failed to mount or format filesystem \n");
        } 
        else if (ret == ESP_ERR_NOT_FOUND) 
        {
            printf( "Failed to find SPIFFS partition \n");
        } else 
        {
            printf( "Failed to initialize SPIFFS (%s) \n", esp_err_to_name(ret));
        }
        return false;
    }
    
    
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) 
    {
        return_value = false;
        printf( "Failed to get SPIFFS partition information (%s) \n", esp_err_to_name(ret));
    } 
    else 
    {
        return_value = true;
        printf( "Partition size: total: %d, used: %d \n", total, used);
    }
    return return_value;
    
}

int format_spiffs_drive(void)
{
    
   int error_code;
  
  
   esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };
   if ( esp_spiffs_mounted(NULL) != false )
   {
       
       error_code = esp_vfs_spiffs_unregister(NULL);
       
   } 
   error_code = esp_spiffs_format(NULL);
   //printf("error code spiffs %d \n",error_code);
   error_code = esp_vfs_spiffs_register(&conf);
    //printf("error coderegister %d \n",error_code);
    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.   
    return error_code;
}   
    
   