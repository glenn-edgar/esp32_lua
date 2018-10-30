

#include <esp_types.h>
#include <stdio.h>
#include "cmp.h"

#include "esp_spiffs.h"
#include "msg_dict_stream.h"
#include "msg_command_cmd_table.h"
#include "msg_command_data.h"
#include "esp_system.h"
#include <dirent.h>
#include "msgpack_rx_handler.h"
#include "watchdog.h"
#include "spiffs_setup.h"

#include "msg_process_builtin_cmds.h"


#define FILE_MOUNT_SIZE 16
#define FILE_SIZE 33
#define DATA_SIZE 2000


static void add_commands(void);
/*
    build in commands

*/

static bool format_spiffs( struct cmp_ctx_s *ctx,
                           int *msg_pack_number, 
                           MSG_PACK_ELEMENT **msg_pack); 
static bool file_write(struct cmp_ctx_s *ctx, 
                       int *msg_pack_number, 
                        MSG_PACK_ELEMENT **msg_pack);                         

                        
static bool file_read( struct cmp_ctx_s *ctx,
                      int *msg_pack_number, 
                       MSG_PACK_ELEMENT **msg_pack);                                              
                       
static bool file_delete( struct cmp_ctx_s *ctx,
                         int *msg_pack_number, 
                         MSG_PACK_ELEMENT **msg_pack);                                                 
                        
static bool file_rename( struct cmp_ctx_s *ctx,
                         int *msg_pack_number, 
                         MSG_PACK_ELEMENT **msg_pack);                                                  
                        
static bool file_directory( struct cmp_ctx_s *ctx,
                          int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack);                                                       
                         
static bool reboot( 
                         struct cmp_ctx_s *ctx,
                        int *msg_pack_number, 
                       MSG_PACK_ELEMENT **msg_pack);                                                
                       
static bool heap_space( struct cmp_ctx_s *ctx,
                         int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack);                                                      
                          
static bool list_commands(struct cmp_ctx_s *ctx,
                         int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack);                                                     
    

static bool get_wifi_mac_address(struct cmp_ctx_s *ctx,
                                  int *msg_pack_number, 
                                  MSG_PACK_ELEMENT **msg_pack);
    

void msg_command_data_initialize(void)
{
    initialize_command_table( );
    add_commands();
   
}


/*
****************************************** static functions *********************************************
*/
/*
    storing built in command
*/

static void add_commands(void)
{
   msg_command_add_command("FILE_WRITE",file_write) ;
   msg_command_add_command("FILE_READ",file_read)   ; 
   msg_command_add_command("FILE_DELETE",file_delete) ; 
   msg_command_add_command("FILE_DIR",file_directory) ; 
   msg_command_add_command("FILE_RENAME",file_rename);
   msg_command_add_command("REBOOT",reboot) ; 
   msg_command_add_command("HEAP_SPACE",heap_space);  
   msg_command_add_command("LIST_COMMANDS",list_commands);
   msg_command_add_command("GET_WIFI_MAC_ADDRESS",get_wifi_mac_address);   
   msg_command_add_command("FORMAT_SPIFFS",format_spiffs);                  
    
}

static  uint8_t mac_addr[7];
static bool get_wifi_mac_address(struct cmp_ctx_s *ctx,
                                int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack)

{
  
   MSG_PACK_ELEMENT *temp;
   memset(mac_addr,0,sizeof(mac_addr));
   temp = malloc(sizeof(MSG_PACK_ELEMENT)*3);
   *msg_pack = temp;
   *msg_pack_number = 3;
   
   msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
   msg_dict_pack_string(&temp[1],"COMMAND", "WIFI_MAC_ADDRESS");
   esp_efuse_mac_get_default(mac_addr);
  
   msg_dict_pack_binary(&temp[2],"DATA", mac_addr, 6); 
   
   
   return true;
}


static bool file_write(  struct cmp_ctx_s *ctx, 
                         int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack)

{
    
    
    uint32_t size;
    char *temp_pointer;
    uint32_t temp_length;
    FILE *spiffs_file;
    char *file_name;
    MSG_PACK_ELEMENT *temp;
    int nwrite;

    
    *msg_pack_number = 0;
 
    printf("file write \n");
    
    size = FILE_SIZE;
   
    
    file_name = malloc(size);
    memset(file_name,0,sizeof(size));


    
    
    if( msgpack_rx_handler_find_string(ctx,"FILE_NAME", file_name, &size) != true )
    {
       printf("file name not found %s \n",file_name);
       free(file_name);
       return false;
        
    }
    
    //printf("file name found %s %d \n\n",file_name,size);
    if( msgpack_rx_handler_find_object(ctx,"FILE_DATA", &temp_pointer, &temp_length) != true)
    {
       printf("file data not found \n");
       free(file_name);
       return false;
        
    }
    
    spiffs_file = fopen(file_name,"wb");
    
    nwrite = 0;
    
    if(spiffs_file != NULL)
    {
      nwrite = fwrite(temp_pointer,1,temp_length,spiffs_file);

      temp = malloc(sizeof(MSG_PACK_ELEMENT)*6);
      *msg_pack = temp;
      *msg_pack_number = 6;
      msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
      msg_dict_pack_string(&temp[1],"COMMAND", "FILE_WRITE");
      msg_dict_pack_map(&temp[2],"DATA",3);
      msg_dict_pack_string_dynamic(&temp[3],"FILE_NAME", file_name);
      msg_dict_pack_boolean(&temp[4],"STATUS",true);
      msg_dict_pack_unsigned_integer(&temp[5],"BYTES_WRITE", nwrite);
      fclose(spiffs_file);
     
    }
    else
    {
    printf("file not opened %s \n",file_name);
    temp = malloc(sizeof(MSG_PACK_ELEMENT)*6);
    *msg_pack = temp;
    *msg_pack_number = 6;
    msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
    msg_dict_pack_string(&temp[1],"COMMAND", "FILE_WRITE");
    msg_dict_pack_map(&temp[2],"DATA",3);
    msg_dict_pack_string_dynamic(&temp[3],"FILE_NAME", file_name);
    msg_dict_pack_boolean(&temp[4],"STATUS",false);
    msg_dict_pack_unsigned_integer(&temp[5],"BYTES_WRITE", nwrite);
    }   
    
    return true;      

}    
   

static bool file_read(   struct cmp_ctx_s *ctx,
                         int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack)

{
    
    
    uint32_t size;
    char *data_ptr;
    uint32_t data_length;
    FILE *spiffs_file;
    char *file_name;
    MSG_PACK_ELEMENT *temp;
    int nread;

    
    *msg_pack_number = 0;
 

    
    
    
    
    file_name = malloc(FILE_SIZE);
    memset(file_name,0,sizeof(size));
    size = FILE_SIZE;
    data_ptr = malloc(DATA_SIZE);
    memset(data_ptr,0,DATA_SIZE);
    data_length = DATA_SIZE;
    
     
    
    
    //printf("file read \n");
    if( msgpack_rx_handler_find_string(ctx,"FILE_NAME", file_name, &size) != true )
    {
       free(file_name);
       printf("file name not found \n");
       return false;
        
    }
    

    spiffs_file = fopen(file_name,"rb");
    nread = 0;
    if(spiffs_file != NULL)
    {
      nread = fread(data_ptr,1,data_length,spiffs_file);
      
      temp = malloc(sizeof(MSG_PACK_ELEMENT)*6);
      *msg_pack = temp;
      *msg_pack_number = 6;
      msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
      msg_dict_pack_string(&temp[1],"COMMAND", "FILE_READ");
      msg_dict_pack_map(&temp[2],"DATA",3);
      msg_dict_pack_string_dynamic(&temp[3],"FILE_NAME", file_name);
      msg_dict_pack_boolean(&temp[4],"STATUS",true);
      msg_dict_pack_binary_dynamic(&temp[5],"FILE_DATA", data_ptr,nread);

      fclose(spiffs_file);
     
    }
    else
    {

      temp = malloc(sizeof(MSG_PACK_ELEMENT)*6);
      *msg_pack = temp;
      *msg_pack_number = 6;
      msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
      msg_dict_pack_string(&temp[1],"COMMAND", "FILE_READ");
      msg_dict_pack_map(&temp[2],"DATA",3);
      msg_dict_pack_string_dynamic(&temp[3],"FILE_NAME", file_name);
      msg_dict_pack_boolean(&temp[4],"STATUS",false);
      msg_dict_pack_null(&temp[5],"FILE_DATA");

      
     
    }
        
    return true;
}    


static bool file_delete( struct cmp_ctx_s *ctx,
                          int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack)
{
    
     
    uint32_t size;
    
    char *file_name;
    MSG_PACK_ELEMENT *temp;
    
    bool status;
    int error_code;

    
    *msg_pack_number = 0;
 

    
    
    
    
    file_name = malloc(FILE_SIZE);
    memset(file_name,0,sizeof(size));
    size = FILE_SIZE;
    
    
    
    
    
    //printf("file read \n");
    if( msgpack_rx_handler_find_string(ctx,"FILE_NAME", file_name, &size) != true )
    {
       printf("file name not found \n");
       return false;
        
    }
    

    
    
    
    error_code =remove(file_name);
    if( error_code == 0)
    {
        status = true;
    } 
    else
    {
        status = false;
    }        
    //printf("remove file %d \n",remove(file_name));    
    
    temp = malloc(sizeof(MSG_PACK_ELEMENT)*6);
    *msg_pack = temp;
    *msg_pack_number = 6;
    msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
    msg_dict_pack_string(&temp[1],"COMMAND", "FILE_DELETE");
    msg_dict_pack_map(&temp[2],"DATA",3);
    msg_dict_pack_string_dynamic(&temp[3],"FILE_NAME", file_name);
    msg_dict_pack_boolean(&temp[4],"STATUS",status);
    msg_dict_pack_integer(&temp[5],"ERROR_CODE",error_code);
    return true;
 
}    


static bool file_rename( struct cmp_ctx_s *ctx,
                         int *msg_pack_number, 
                         MSG_PACK_ELEMENT **msg_pack)                                               

{
    
     
    uint32_t to_size;
    uint32_t from_size;
    
    char *from_name;
    char *to_name;
    MSG_PACK_ELEMENT *temp;
    
    bool status;

    
    *msg_pack_number = 0;
 

    
    
    
    from_size = FILE_SIZE;
    from_name = malloc(FILE_SIZE);
    memset(from_name,0,from_size);
    to_size = FILE_SIZE;   
    to_name = malloc(FILE_SIZE);
    memset(to_name,0,to_size);
    
    
     
    
    
    //printf("file read \n");
    if(msgpack_rx_handler_find_string(ctx,"TO_NAME", to_name, &to_size) != true )
    {
       printf("to name not found \n");
       return false;
        
    }
    if(msgpack_rx_handler_find_string(ctx,"FROM_NAME", from_name, &from_size) != true )
    {
       printf("to name not found \n");
       return false;
        
    }
     

    
    status = false;
    
    
    if( rename(from_name, to_name) == 0)
    {
        status = true;
    }        
    
    temp = malloc(sizeof(MSG_PACK_ELEMENT)*6);
    *msg_pack = temp;
    *msg_pack_number = 6;
    msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
    msg_dict_pack_string(&temp[1],"COMMAND", "FILE_RENAME");
    msg_dict_pack_map(&temp[2],"DATA",2);
    msg_dict_pack_string_dynamic(&temp[3],"TO_NAME", to_name);
    msg_dict_pack_string_dynamic(&temp[4],"FROM_NAME", from_name);
    msg_dict_pack_unsigned_integer(&temp[5],"STATUS", status);

    return true;
 
}    


static int count_directory(char *file_mount )
{ 
  
   struct dirent *de;  // Pointer for directory entry
   DIR *dr;
   int count;
   
   
   count = 0;
  
   dr   = opendir(file_mount);
   if (dr == NULL)  // opendir returns NULL if couldn't open directory
   {
        printf("Could not open current directory \n" );
        return -1;
   }
   
  while ((de = readdir(dr)) != NULL)
  {
            count += 1;
            //printf("name %s\n", de->d_name);
            
           
  }
   closedir(dr); 
   return count;   
    
}    

static int list_directory(int max_count, MSG_PACK_ELEMENT *element, char *file_mount )
{ 
  
   struct dirent *de;  // Pointer for directory entry
   DIR *dr;
   int count;
   char *file_name;
   int file_length;
   
   
   count = 0;
  
   dr   = opendir(file_mount);
   if (dr == NULL)  // opendir returns NULL if couldn't open directory
   {
        printf("Could not open current directory \n" );
       return -1;
   }
   
  while ((de = readdir(dr)) != NULL)
  {
            count += 1;
            //printf("name %s\n", de->d_name);
            file_length = strlen(de->d_name)+4;
            file_name = malloc(file_length);
            memset(file_name,0,file_length);
            strcpy(file_name,de->d_name);
            msg_dict_pack_string_dynamic(element,NULL,file_name);
            element++;
           
            if(count >= max_count)
            {
                break;
            }
           
  }
   closedir(dr); 
   return count;   
    
}  

static bool file_directory( struct cmp_ctx_s *ctx,
                          int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack) 

{
     
   
    MSG_PACK_ELEMENT *temp;
    int pack_elements;
    int file_elements;
    char *file_mount;
    uint32_t size;
    
    file_mount = malloc(FILE_MOUNT_SIZE+1);
    memset(file_mount, 0, FILE_MOUNT_SIZE+1);
    size = FILE_MOUNT_SIZE;
 
    
   

    if( msgpack_rx_handler_find_string(ctx,"FILE_MOUNT", file_mount, &size) != true )
    {
       printf("directory name not found \n");
       free(file_mount);
       return false;
        
    }
    
  
   
   
   file_elements =  count_directory(file_mount);
   if( file_elements <0){ return false; }
   pack_elements = file_elements +3;
   temp = malloc(sizeof(MSG_PACK_ELEMENT)*pack_elements);
   *msg_pack = temp;
   *msg_pack_number = pack_elements;
   
   msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
   msg_dict_pack_string(&temp[1],"COMMAND", "FILE_DIR");
 
   msg_dict_pack_array(&temp[2],"DATA", file_elements);
   list_directory(file_elements, &temp[3],file_mount );
   free(file_mount); 
   return true;
}




static bool reboot(       struct cmp_ctx_s *ctx,
                          int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack) 

{
    abort();
    return false;
}
static bool heap_space(  struct cmp_ctx_s *ctx, 
                         int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack) 

{
    
   MSG_PACK_ELEMENT *temp;
   
   
   
   
   
   temp = malloc(sizeof(MSG_PACK_ELEMENT)*3);
   *msg_pack = temp;
   *msg_pack_number = 3;
   
   msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
   msg_dict_pack_string(&temp[1],"COMMAND", "HEAP_SPACE");
   
  
   msg_dict_pack_unsigned_integer(&temp[2],"DATA",esp_get_free_heap_size());
   return true; 
   
    
    
}
static bool list_commands( struct cmp_ctx_s *ctx,
                         int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack)                                                

{
    
   MSG_PACK_ELEMENT *temp;
   int pack_elements;
   int command_elements;
   
   command_elements =  msg_command_command_number() ;
   pack_elements = command_elements +3;
   temp = malloc(sizeof(MSG_PACK_ELEMENT)*pack_elements);
   *msg_pack = temp;
   *msg_pack_number = pack_elements;
   
   msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
   msg_dict_pack_string(&temp[1],"COMMAND", "LIST_COMMANDS");
 
   msg_dict_pack_array(&temp[2],"DATA", command_elements);
   for(int i=0; i<command_elements;i++)
   {
       msg_dict_pack_string(&temp[i+3 ],NULL, msg_command_list_cmd_name(i));
   }
   return true;
}   
 
static bool format_spiffs( struct cmp_ctx_s *ctx,
                           int *msg_pack_number, 
                           MSG_PACK_ELEMENT **msg_pack)
{
   int error_code;
   bool status;
   MSG_PACK_ELEMENT *temp;
   
   wdt_task_unsubscribe();
   error_code =  format_spiffs_drive();
 
   wdt_task_subscribe();
   if(error_code == 0 )
   {
      status = true;    
       
   }
   else
   {
       status = false;
   }
   temp = malloc(sizeof(MSG_PACK_ELEMENT)*5);
   *msg_pack = temp;
   *msg_pack_number = 5;
   
   msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
   msg_dict_pack_string(&temp[1],"COMMAND", "FORMAT_SPIFFS");
   msg_dict_pack_map(&temp[2],"DATA",2);
   msg_dict_pack_boolean(&temp[3],"STATUS",status);
   msg_dict_pack_integer(&temp[4],"ERROR_CODE",error_code);

   
   return true;
}