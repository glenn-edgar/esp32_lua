#ifndef _MSG_DICT_STREAM_H_
#define _MSG_DICT_STREAM_H_

#include <string.h>

typedef enum {MSGPACK_INT_TYPE = 0, 
               MSGPACK_UINT_TYPE = 1, 
               MSGPACK_FLOAT_TYPE = 2, 
               MSGPACK_STR_TYPE = 3, 
               MSGPACK_BIN_TYPE = 4,
               MSGPACK_ARRAY_TYPE = 5,
               MSGPACK_MAP_TYPE = 6,
               MSGPACK_BOOLEAN_TYPE = 7,
               MSGPACK_NULL_TYPE = 8,
               } MSG_TYPE;
 

typedef struct
{
  MSG_TYPE type;
  union {
  long integer;
  unsigned long uinteger;
  char *string;
  double real;
  void *binary;
  } data;
  int size; // needed by binary element
  char *field_name;
  int malloc_flag;
}MSG_PACK_ELEMENT;





static inline void msg_dict_pack_integer(MSG_PACK_ELEMENT *element,char *field_name, long data)
{
    element->field_name = field_name;
    element->type = MSGPACK_INT_TYPE;
    element->data.integer = data;
    element->malloc_flag = false;  
}

static inline void msg_dict_pack_unsigned_integer(MSG_PACK_ELEMENT *element,char *field_name, unsigned long data)
{
    element->field_name = field_name;
    element->type = MSGPACK_UINT_TYPE;
    element->data.uinteger = data;
    element->malloc_flag = false;
}

static inline void msg_dict_pack_string(MSG_PACK_ELEMENT *element,char *field_name, char *data)
{
    element->field_name = field_name;
    element->type = MSGPACK_STR_TYPE;
    element->data.string = data;
    element->size = strlen(data);    
    element->malloc_flag = false;
}

static inline void msg_dict_pack_string_dynamic(MSG_PACK_ELEMENT *element,char *field_name, char *data)
{
    element->field_name = field_name;
    element->type = MSGPACK_STR_TYPE;
    element->data.string = data;
    element->size = strlen(data);    
    element->malloc_flag = true;
    
}


static inline void msg_dict_pack_binary(MSG_PACK_ELEMENT *element, char *field_name,void *data, int size )
{
    element->field_name = field_name;
    element->type = MSGPACK_BIN_TYPE;
    element->data.binary = data;
    element->size = size ;
    element->malloc_flag = false;
    
}

static inline void msg_dict_pack_binary_dynamic(MSG_PACK_ELEMENT *element, char *field_name,void *data, int size )
{
    element->field_name = field_name;
    element->type = MSGPACK_BIN_TYPE;
    element->data.binary = data;
    element->size = size ;
    element->malloc_flag = true;
    
}

static inline void msg_dict_pack_float(MSG_PACK_ELEMENT *element,char *field_name, double data)
{
    element->field_name = field_name;
    element->type = MSGPACK_FLOAT_TYPE;
    element->data.real = data;
    element->malloc_flag = false;    
  

    
    
}



static inline void msg_dict_pack_array(MSG_PACK_ELEMENT *element,char *field_name, int array_size)
{
    element->field_name = field_name;
    element->type = MSGPACK_ARRAY_TYPE;
    element->size = array_size;
    element->malloc_flag = false;       
}

static inline void msg_dict_pack_map(MSG_PACK_ELEMENT *element,char *field_name, int map_size)
{
    element->field_name = field_name;
    element->type = MSGPACK_MAP_TYPE;
    element->size = map_size;
    element->malloc_flag = false;       
}

static inline void msg_dict_pack_null(MSG_PACK_ELEMENT *element,char *field_name)
{
    element->field_name = field_name;
    element->type = MSGPACK_NULL_TYPE;
    element->size = 0;
    element->malloc_flag = false;       
}
static inline void msg_dict_pack_boolean(MSG_PACK_ELEMENT *element,char *field_name, int status)
{
    element->field_name = field_name;
    element->data.integer = status;
    element->type = MSGPACK_BOOLEAN_TYPE;
    element->size = 0;
    element->malloc_flag = false;       
}

char *msg_dict_stream( int *buffer_size, int number, MSG_PACK_ELEMENT *msg_pack);


#endif
