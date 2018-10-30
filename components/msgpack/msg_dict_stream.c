#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmp.h"
#include "msg_dict_stream.h"






static int allocate_buffer( int number, MSG_PACK_ELEMENT *msg_pack );
static int find_map_number(int number, MSG_PACK_ELEMENT *msg_pack );





static void msgpack_dict_stream_init(cmp_ctx_t *ctx, char *write_buffer, int write_size);

char * msg_dict_stream(  int *buff_size, int number,  MSG_PACK_ELEMENT *msg_pack)

{
  cmp_ctx_t ctx;
  int map_number;
  int output_size;
  
  char *output_buffer;
  
  map_number = find_map_number(number,msg_pack);
  output_size = allocate_buffer( number, msg_pack );
  output_buffer = malloc(output_size);
  msgpack_dict_stream_init(&ctx, output_buffer, output_size); 
  
  cmp_write_map(&ctx,map_number);
 
  for(int i=0; i<number;i++)
  {

    //printf("input packet %d %d \n",i,msg_pack->type);
    if(msg_pack->field_name != NULL )
    {
      cmp_write_str(&ctx,msg_pack->field_name ,strlen(msg_pack->field_name) );
    }
    else
    {
        ;//printf("null field %d \n",msg_pack->type);
    }

    switch(msg_pack->type)
    {
        
        case MSGPACK_MAP_TYPE:
          cmp_write_map(&ctx,msg_pack->size);
          break;
        case MSGPACK_STR_TYPE: // string data
          cmp_write_str(&ctx,msg_pack->data.string ,msg_pack->size );
          if(msg_pack->malloc_flag == true)
          {
             free(msg_pack->data.string);
          }
          break;
          
          
        case MSGPACK_INT_TYPE: // integer data
          cmp_write_integer(&ctx , msg_pack->data.integer);
           break;
           
        case MSGPACK_UINT_TYPE: // integer data
           cmp_write_uinteger(&ctx, msg_pack->data.uinteger);
           break;


           
        case MSGPACK_FLOAT_TYPE: // float data
          cmp_write_decimal(&ctx, msg_pack->data.real);
           break;
        
       case MSGPACK_BIN_TYPE:  //binary data
          

          cmp_write_bin(&ctx, msg_pack->data.binary ,msg_pack->size );
          if(msg_pack->malloc_flag == true)
          {
             free(msg_pack->data.string);
          }
          
           break;
           
       case MSGPACK_ARRAY_TYPE:
              cmp_write_array(&ctx, msg_pack->size);
             break;
             
        case  MSGPACK_BOOLEAN_TYPE:
             cmp_write_bool(&ctx,msg_pack->data.integer); 
             break;             
        
       case MSGPACK_NULL_TYPE :     
             cmp_write_nil(&ctx);
            break;
            
       
       default:
       
         abort();
    }
    msg_pack++;
    
           
  }
  *buff_size = ctx.ctx_buf_ctl.index;
  
  return output_buffer;
}  
    
    

static int allocate_buffer( int number, MSG_PACK_ELEMENT *msg_pack )
{
    int size =0;
   
    size = 10; //allocate map
    for(int i=0;i<number;i++)
    {
      if( msg_pack->field_name != NULL)
      {
         size += strlen(msg_pack->field_name) +6;
      }
      switch(msg_pack->type)
      {
        case MSGPACK_MAP_TYPE:
          size += 6;
        break;

        case MSGPACK_STR_TYPE: // string data
          size += (msg_pack->size + 6);
          break;
          
          
        case MSGPACK_INT_TYPE: // integer data
          size += 14;
           break;
           
        case MSGPACK_UINT_TYPE: // integer data
           size += 14;
           break;


           
        case MSGPACK_FLOAT_TYPE: // float data
          size += 14;
           break;
        
       case MSGPACK_BIN_TYPE:  //binary data
           size += (msg_pack->size + 6);
           break; 
       
       case MSGPACK_ARRAY_TYPE:
           size += 6;  
           break;

       case  MSGPACK_BOOLEAN_TYPE:
       case MSGPACK_NULL_TYPE :
           size += 1;
           break;           
      }
      msg_pack++;
    }
    size = size +2; //for crc
    return size;
    
    
}

static int find_map_number( int number, MSG_PACK_ELEMENT *msg_pack )
{
    int map_number;
   
    map_number = number;
   
    for(int i=0;i<number;i++)
    {

      switch(msg_pack->type)
      {
        case MSGPACK_MAP_TYPE:
       
          map_number -= msg_pack->size;
        break;


       
       case MSGPACK_ARRAY_TYPE:
         
          map_number -= msg_pack->size;
        break;
  
        default:
           break;
      
      }
      msg_pack++;
    }
    
    return map_number;
}





//typedef size_t (*cmp_writer)(struct cmp_ctx_s *ctx, const void *data,size_t count);
static size_t write_function(struct cmp_ctx_s *ctx, const void *data, size_t count )
{
    
    if((ctx->ctx_buf_ctl.current_pointer+count)>= (ctx->ctx_buf_ctl.buffer +ctx->ctx_buf_ctl.size))
    {
       count = ctx->ctx_buf_ctl.buffer +ctx->ctx_buf_ctl.size-ctx->ctx_buf_ctl.current_pointer;
    }
    memcpy(ctx->ctx_buf_ctl.current_pointer,data,count);
    ctx->ctx_buf_ctl.current_pointer += count;
    ctx->ctx_buf_ctl.index += count;
    return count;
    
}






static void msgpack_dict_stream_init(cmp_ctx_t *ctx, char *write_buffer, int write_size)
{
    ctx->ctx_buf_ctl.buffer = write_buffer;
    ctx->ctx_buf_ctl.current_pointer = write_buffer;
    ctx->ctx_buf_ctl.index = 0;
    ctx->ctx_buf_ctl.size = write_size; 
    cmp_init( ctx, (void *)ctx, NULL,NULL, write_function );
    
}