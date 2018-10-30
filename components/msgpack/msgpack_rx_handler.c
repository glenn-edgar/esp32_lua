#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "cmp.h"
#include "hexadecimal.h"
#include "msgpack_rx_handler.h"

#define FILE_NAME_SIZE 32


bool analyize_current_object(cmp_ctx_t *ctx, cmp_object_t *obj,uint32_t *skip_number, uint32_t *size);
static char *cmp_skip_function(struct cmp_ctx_s *ctx,  void *data, size_t count);
static bool   cmp_reader_function(struct cmp_ctx_s *ctx, void *data, size_t number_to_read);
static void reset_buffer(cmp_ctx_t *ctx);
static char *cmp_return_current_buffer(struct cmp_ctx_s *ctx);


bool msgpack_rx_handler_file(cmp_ctx_t *ctx,char *filename, char **buffer,uint32_t *buffer_size )
{
    
   
    char *temp_ptr;
    FILE *file_handle;
    char *file_buffer;
    
    file_buffer = malloc(FILE_NAME_SIZE);
    strcpy( file_buffer, filename);
    
    file_handle = fopen(file_buffer,"rb");
    
    
    if(file_handle == NULL)
    {
        free(file_buffer);
        return false;
       
    }
    
    temp_ptr = malloc(*buffer_size);
    *buffer_size = fread(temp_ptr,1,*buffer_size,file_handle);

    fclose(file_handle);
    *buffer = temp_ptr;
    if(*buffer_size <= 0)
    {
        
        free(temp_ptr);
        free(file_buffer);
        return false;
    }

    
    free(file_buffer);
    msgpack_rx_handler_init(ctx, temp_ptr, *buffer_size);
    return true;
}

void msgpack_rx_handler_init(cmp_ctx_t *ctx, char *read_buffer, int read_size)
{
    ctx->ctx_buf_ctl.buffer = read_buffer;
    ctx->ctx_buf_ctl.current_pointer = read_buffer;
    ctx->ctx_buf_ctl.index = 0;
    ctx->ctx_buf_ctl.size = read_size; 
    cmp_init( ctx, (void *)ctx, cmp_reader_function, cmp_skip_function, NULL );
    
}



bool msgpack_rx_handler_find_object(cmp_ctx_t *ctx,char *field_name, char **buffer, uint32_t *buffer_size)
{
     bool return_value;
     
     char *temp_pointer = NULL;
     uint32_t   temp_size =0;
     uint32_t   map_size;
  
    *buffer_size = 0;
    
    reset_buffer(ctx);
    
    
    if( cmp_read_map(ctx, &map_size) != true) {return false;}
   

   
    for(int i = 0;i < map_size; i++)
    { 

       
       return_value =  msgpack_rx_handler_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
       
       if(return_value != true){return return_value;}

       if( ctx_strcmp(field_name,temp_pointer,temp_size ) == true)
       {
           
           return_value = msgpack_rx_handler_scoop_field(ctx,(void **)buffer,buffer_size);
           
           return return_value;
           
           
       }
   
       msgpack_rx_handler_skip_field(ctx); 
  
        
    }
    
    return false;
}    


bool msgpack_rx_handler_find_string(cmp_ctx_t *ctx,char *field_name, char *string, uint32_t *size )
{
    bool return_value;
   
    char *temp_pointer;
    uint32_t   temp_size;
    uint32_t   map_size;

    
    reset_buffer(ctx);
    
    
    if( cmp_read_map(ctx, &map_size) != true) {return false;}
  
    
    for(int i = 0;i < map_size; i++)
    {  
       
   
       return_value =  msgpack_rx_handler_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
   
       if(return_value != true){return return_value;}
       //printf("@@@@@@@@@@@@@@@ field %s  msg pack field %.*s   \n",field_name,temp_size,temp_pointer);
       if( ctx_strcmp(field_name,temp_pointer,temp_size ) == true)
       {
          
           memset(string,0,*size);
           //printf("string size %d \n",*size);
           return_value = cmp_read_str(ctx, string, size);
           if(return_value != true)
           {
               //printf("string size problem %d \n",*size);
           }
           
           return return_value;
           
           
       }
 
       msgpack_rx_handler_skip_field(ctx); 
            
    } 
    return false;
}

bool msgpack_rx_handler_find_integer(cmp_ctx_t *ctx,char *field_name, int32_t *data )
{
    bool return_value;
   
    char *temp_pointer;
    uint32_t   temp_size;
    uint32_t   map_size;

    
    reset_buffer(ctx);
    
    
    if( cmp_read_map(ctx, &map_size) != true) {return false;}
  
    
    for(int i = 0;i < map_size; i++)
    {  
       
       
       return_value =  msgpack_rx_handler_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
       
       if(return_value != true){return return_value;}
       
       if( ctx_strcmp(field_name,temp_pointer,temp_size ) == true)
       {
          
          
          
           return_value = cmp_read_int(ctx,data);
           
           
           return return_value;
           
           
       }
 
       msgpack_rx_handler_skip_field(ctx); 
            
    } 
    return false;
}


bool msgpack_rx_handler_find_float(cmp_ctx_t *ctx,char *field_name, float *data )
{
    bool return_value;
   
    char *temp_pointer;
    uint32_t   temp_size;
    uint32_t   map_size;

    
    reset_buffer(ctx);
    
    
    if( cmp_read_map(ctx, &map_size) != true) {return false;}
  
    //printf("map size is %d \n",map_size);
    for(int i = 0;i < map_size; i++)
    {  
       
       
       return_value =  msgpack_rx_handler_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
       
       if(return_value != true){return return_value;}
       
       if( ctx_strcmp(field_name,temp_pointer,temp_size ) == true)
       {
          
          
          
           return_value = cmp_read_float(ctx,data);
           
           
           return return_value;
           
           
       }
 
       msgpack_rx_handler_skip_field(ctx); 
            
    } 
    return false;
}


bool msgpack_rx_handler_find_unsigned(cmp_ctx_t *ctx,char *field_name, uint32_t *data )
{
    bool return_value;
   
    char *temp_pointer;
    uint32_t   temp_size;
    uint32_t   map_size;

    
    reset_buffer(ctx);
    
    
    if( cmp_read_map(ctx, &map_size) != true) {return false;}
  
    
    for(int i = 0;i < map_size; i++)
    {  
       
       
       return_value =  msgpack_rx_handler_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
       
       if(return_value != true){return return_value;}
       
       if( ctx_strcmp(field_name,temp_pointer,temp_size ) == true)
       {
          
           
          
           return_value = cmp_read_uint(ctx,data);
           
           
           return return_value;
           
           
       }
 
       msgpack_rx_handler_skip_field(ctx); 
            
    } 
    return false;
}

bool msgpack_rx_handler_find_binary(cmp_ctx_t *ctx,char *field_name, char *buffer, uint32_t *buffer_size)
{
    bool return_value;
   
    char *temp_pointer;
    uint32_t   temp_size;
    uint32_t   map_size;

    
    reset_buffer(ctx);
    
    
    if( cmp_read_map(ctx, &map_size) != true) {return false;}
  
    
    for(int i = 0;i < map_size; i++)
    {  
       
       
       return_value =  msgpack_rx_handler_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
       
       if(return_value != true){return return_value;}
       
       if( ctx_strcmp(field_name,temp_pointer,temp_size ) == true)
       {
          
           
          
           return_value = cmp_read_bin(ctx, buffer, buffer_size );
           
           
           return return_value;
           
           
       }
 
       msgpack_rx_handler_skip_field(ctx); 
            
    } 
    return false;
}

bool msgpack_rx_handler_find_array_count(cmp_ctx_t *ctx,char *field_name,uint32_t *count)
{
    bool return_value;
   
    char *temp_pointer;
    uint32_t   temp_size;
    uint32_t   map_size;

    
    reset_buffer(ctx);
    
    
    if( cmp_read_map(ctx, &map_size) != true) {return false;}
  
    
    for(int i = 0;i < map_size; i++)
    {  
       
       
       return_value =  msgpack_rx_handler_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
       
       if(return_value != true){return return_value;}
       
       if( ctx_strcmp(field_name,temp_pointer,temp_size ) == true)
       {
          
           
          
           return_value =  cmp_read_array(ctx,count);
           
           
           return return_value;
           
           
       }
 
       msgpack_rx_handler_skip_field(ctx); 
            
    } 
    return false;
}


bool msgpack_rx_handler_find_array_unsigned(cmp_ctx_t *ctx,char *field_name,uint32_t *count, uint32_t *buffer)
{
    bool return_value;
   
    char *temp_pointer;
    uint32_t   temp_size;
    uint32_t   map_size;
    uint32_t   size;

    
    reset_buffer(ctx);
    
    
    if( cmp_read_map(ctx, &map_size) != true) {return false;}
  
    
    for(int i = 0;i < map_size; i++)
    {  
       
       
       return_value =  msgpack_rx_handler_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
       
       if(return_value != true){return return_value;}
       
       if( ctx_strcmp(field_name,temp_pointer,temp_size ) == true)
       {
          
           
          
           return_value =  cmp_read_array(ctx,&size);
           if( size != *count)
           {
               return false;
           }
           for(int j = 0; j < size; j++)
           {
             if( cmp_read_uint(ctx, buffer) !=true)
             {
                 return false;
             }
             buffer++;
           
           }
           
           return true;
           
           
       }
 
       msgpack_rx_handler_skip_field(ctx); 
            
    } 
    return false;
}

bool msgpack_rx_handler_find_array_integer(cmp_ctx_t *ctx,char *field_name,uint32_t *count, int32_t *buffer)
{
    bool return_value;
   
    char *temp_pointer;
    uint32_t   temp_size;
    uint32_t   map_size;
    uint32_t   size;

    
    reset_buffer(ctx);
    
    
    if( cmp_read_map(ctx, &map_size) != true) {return false;}
  
    
    for(int i = 0;i < map_size; i++)
    {  
       
       
       return_value =  msgpack_rx_handler_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
       
       if(return_value != true){return return_value;}
       
       if( ctx_strcmp(field_name,temp_pointer,temp_size ) == true)
       {
          
           
          
           return_value =  cmp_read_array(ctx,&size);
           if( size != *count)
           {
               return false;
           }
           for(int j = 0;j < size; j++)
           {
             if( cmp_read_int(ctx, buffer) !=true)
             {
                 return false;
             }
             buffer++;
           
           }
           
           return true;
           
           
       }
 
       msgpack_rx_handler_skip_field(ctx); 
            
    } 
    return false;
}


bool msgpack_rx_handler_get_binary_array(cmp_ctx_t *ctx,char *field_name,uint32_t *count,  uint32_t *buffer_size,char **buffer)
{
    bool return_value;
   
    char *temp_pointer;
    uint32_t   temp_size;
    uint32_t   map_size;
    uint32_t   size;

    
    reset_buffer(ctx);
    
    
    if( cmp_read_map(ctx, &map_size) != true) {return false;}
  
    
    for(int i = 0;i < map_size; i++)
    {  
       
       
       return_value =  msgpack_rx_handler_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
       
       if(return_value != true){return return_value;}
       
       if( ctx_strcmp(field_name,temp_pointer,temp_size ) == true)
       {
          
           
          
           return_value =  cmp_read_array(ctx,&size);
           if( size != *count)
           {
               return false;
           }
           for(int j = 0;j < size; j++)
           {
             if( msgpack_rx_handler_scoop_field(ctx, (void **) &buffer[j], &buffer_size[j]) == false)
             {
                 return false;
             }
            
             //printf("%d buffer size %d buffer pointer %p \n",j, buffer_size[i],buffer[j]);
           }
           
           return true;
           
           
       }
 
       msgpack_rx_handler_skip_field(ctx); 
            
    } 
    return false;
}


bool msgpack_rx_handler_get_bin_data_ptr(cmp_ctx_t *ctx ,void **data, uint32_t *size )
{
    bool return_value;
    
    return_value =  cmp_read_bin_size(ctx, size);
    
    if(return_value == true)
    {   
        *data =  ctx->skip(ctx,NULL, *size);
        
    } 
    return return_value;
    
    
    
    
}


bool msgpack_rx_handler_skip_field(cmp_ctx_t *ctx)
{
    bool return_value;
    cmp_object_t obj;
    uint32_t          size;
    uint32_t          number_to_skip = 1;
    
    return_value = true;
    
    while((number_to_skip > 0) && (return_value == true))
    {
        
        number_to_skip -= 1;
        return_value = analyize_current_object(ctx, &obj, &number_to_skip, &size);
        
        if( return_value == true)
        {
            ctx->skip( ctx,NULL, size ); 
      
        }
    }
    
    return return_value;
    
}

bool  msgpack_rx_handler_scoop_field(cmp_ctx_t  *ctx, void **data, uint32_t *size)
{
    bool return_value;
    cmp_object_t obj;
   
    uint32_t    number_to_skip = 1;
   
    uint32_t    temp_size;
    uint32_t    temp_data_size =0;
    
    
    number_to_skip -= 1;
    *data = cmp_return_current_buffer(ctx);
   
    
    return_value = analyize_current_object(ctx, &obj, &number_to_skip, &temp_size);
    
    if( return_value == true)
    {
          ctx->skip( ctx,NULL,temp_size );   
    }
    if(return_value == false){return false; }
    return_value = true;
    while((number_to_skip > 0) && (return_value == true))
    {
        number_to_skip -= 1;
        return_value = analyize_current_object(ctx, &obj, &number_to_skip, &temp_size);
        temp_data_size += temp_size;
        if( return_value == true)
        {
            ctx->skip( ctx,NULL, temp_size );   
        }
        else
        {
            return false;
        }
        
    }
    
    
   *size = cmp_return_current_buffer(ctx) - (char *)*data;
    //printf("############################## size %d data pointer %p \n",*size,*data);
    return return_value;
    
    
}

bool ctx_strcmp(char *normal_string,char *binary_string,int binary_size )
{
    int string_len;
    
    string_len = strlen(normal_string);
   
    if( string_len != binary_size){ return false;}
    for(int i=0;i<string_len;i++)
    {
        
        if(*normal_string++ != *binary_string++)
        {
            return false;
        }
        
    }
    return true;
    
}
   
void ctx_dump_buffer( cmp_ctx_t  *ctx, int size)
{
    char *hex_buffer;
    int  hex_size;
    CTX_BUF_CTL  *buf_ctl;
    
    buf_ctl = &ctx->ctx_buf_ctl;
    if(size >= (buf_ctl->size -buf_ctl->index) )
    {
        size = buf_ctl->size-buf_ctl->index;
        
    }  
    hex_size = size *2;
    hex_buffer = malloc(hex_size+1);
    memset(hex_buffer,0,hex_size+1);
    binary_to_hex(buf_ctl->current_pointer,size, hex_buffer);
    printf("dumping remaining part of buffer size %d \n",size);
    printf("\n\n+++++++++++++++++\n");
    printf(hex_buffer);
    printf("\n\n---\n");
    free(hex_buffer);
    
}  

/************************************** Static Functions ************************************/

static void reset_buffer(cmp_ctx_t *ctx)
{
   ctx->ctx_buf_ctl.index = 0;
   ctx->ctx_buf_ctl.current_pointer = ctx->ctx_buf_ctl.buffer;
  
}
  
static char *cmp_return_current_buffer(struct cmp_ctx_s *ctx)
{
    return ctx->ctx_buf_ctl.current_pointer;
}

static bool   cmp_reader_function(struct cmp_ctx_s *ctx, void *data, size_t number_to_read)
{
   
    CTX_BUF_CTL *buf_ctl; 
    char *return_buffer;
    
    buf_ctl  = &ctx->ctx_buf_ctl;
    return_buffer = (char *)data;

    
    if((buf_ctl->size)<(buf_ctl->index+number_to_read))
    {
        return false;
    }
    for( int i = 0;i<number_to_read;i++)
    {
       *return_buffer++ = *buf_ctl->current_pointer++;    
        
    }
    buf_ctl->index += number_to_read;

    return true;
    
}   
    




static char *cmp_skip_function(struct cmp_ctx_s *ctx,  void *data, size_t count)
{
   CTX_BUF_CTL *buf_ctl; 
   char *return_value;
   
   buf_ctl  = &ctx->ctx_buf_ctl;
   return_value = buf_ctl->current_pointer;
   buf_ctl->index = buf_ctl->index +count;
   
   if( buf_ctl->index > buf_ctl->size)
   {
       buf_ctl->index = buf_ctl->size;
   }
   
    buf_ctl->current_pointer = buf_ctl->buffer + buf_ctl->index;
   
   return return_value;
    
}


