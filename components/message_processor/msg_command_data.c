#include <esp_types.h>
#include <stdio.h>
#include "cmp.h"

#include "msg_dict_stream.h"
#include "msg_command_cmd_table.h"
#include "msg_command_data.h"
#include "esp_system.h"
#include <dirent.h>
#include "msgpack_rx_handler.h"

#define FILE_MOUNT_SIZE 16
#define FILE_SIZE 33
#define DATA_SIZE 2000

#define check_return(return_value )\
    if(return_value != true){ return false; }

static bool find_command_data(struct cmp_ctx_s *ctx,
                       char **command_data, uint32_t *command_size,
                       char **msgpack_data, uint32_t *msgpack_size);   

static bool find_command_data(struct cmp_ctx_s *ctx,
                       char **command_data, uint32_t *command_size,
                       char **msgpack_data, uint32_t *msgpack_size);
                       
                       
bool msg_command_process_packet(int *msg_pack_number, 
                                 MSG_PACK_ELEMENT **msg_pack,
                                char * input_packet,int size )
{
    cmp_ctx_t ctx;
    bool return_value;
    char *command_data;
    uint32_t command_size;
    char *msgpack_data;
    uint32_t msgpack_size;
    PROCESS_COMMAND_T command_function;
    
    *msg_pack_number = 0;
   
    msgpack_rx_handler_init(&ctx, input_packet, size);
    
    return_value = find_command_data(&ctx,&command_data,&command_size,
                                      &msgpack_data,&msgpack_size );
    
    if( return_value == true)
    {
        
       
        
        command_function =  msg_command_find_command( command_data,command_size);
        if( command_function != NULL )
        {
            
            msgpack_rx_handler_init(&ctx, msgpack_data,msgpack_size);
            return_value = command_function(&ctx, msg_pack_number, msg_pack);
 
        }
 
        
    }
    else
    {
        printf("command not found \n");
    }
    
    return return_value;
}    
    
    
static bool find_command_data(struct cmp_ctx_s *ctx,
                       char **command_data, uint32_t *command_size,
                       char **msgpack_data, uint32_t *msgpack_size)
{
   bool return_value;
   uint32_t map_size;
   int   leave_count = 3;
  
   char *temp_pointer;
   uint32_t   temp_size;
   
   return_value =  cmp_read_map(ctx, &map_size);
   
   if( return_value != true){ return return_value;}
   
  
   for( int i= 0; i<map_size; i++ )
   {
       
      
       return_value =  msgpack_rx_handler_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
       
       check_return(return_value);
       if( ctx_strcmp(COMMAND_FIELD,temp_pointer,temp_size ) == true)
       {
           printf("found command \n");
           return_value = msgpack_rx_handler_get_bin_data_ptr(ctx,(void **)command_data,command_size );
           check_return(return_value);
           leave_count = leave_count &0x2;
           
       }
       else if( ctx_strcmp(DATA_FIELD,temp_pointer,temp_size ) == true)
       {
           
           printf("found data \n");
           return_value = msgpack_rx_handler_scoop_field(ctx,(void **)msgpack_data,msgpack_size);
           
           check_return(return_value);
           leave_count = leave_count &1;
           
       }
       else
       {
          printf("should not happen \n");
          msgpack_rx_handler_skip_field(ctx); 
           
       }
       if(leave_count == 0)
       {
           return true;
       }    
 
   }
   
   return false;    
}      
 
