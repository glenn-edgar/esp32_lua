#ifndef _MSGPACK_RX_HANDLER_H_
#define _MSGPACK_RX_HANDLER_H_

#include "cmp.h"

#include <esp_types.h>

bool msgpack_rx_handler_file(cmp_ctx_t *ctx,char *filename, char **buffer,uint32_t *buffer_size );
void ctx_dump_buffer( cmp_ctx_t  *ctx, int size);
void msgpack_rx_handler_init(cmp_ctx_t *ctx, char *read_buffer, int read_size);


 
bool ctx_strcmp(char *normal_string,char *binary_string,int binary_size );

bool msgpack_rx_handler_find_string(cmp_ctx_t *ctx,char *field_name, char *string, uint32_t *size );

bool msgpack_rx_handler_find_object(cmp_ctx_t *ctx,char *field_name, char **buffer, uint32_t *buffer_size);

bool msgpack_rx_handler_find_integer(cmp_ctx_t *ctx,char *field_name, int32_t *data );

bool msgpack_rx_handler_find_float(cmp_ctx_t *ctx,char *field_name, float *data );

bool msgpack_rx_handler_find_unsigned(cmp_ctx_t *ctx,char *field_name, uint32_t *data );

bool msgpack_rx_handler_find_binary(cmp_ctx_t *ctx,char *field_name, char *buffer, uint32_t *buffer_size);



bool msgpack_rx_handler_find_array_count(cmp_ctx_t *ctx,char *field_name,uint32_t *count);

bool msgpack_rx_handler_find_array_unsigned(cmp_ctx_t *ctx,char *field_name,uint32_t *count, uint32_t *buffer);

bool msgpack_rx_handler_find_array_integer(cmp_ctx_t *ctx,char *field_name,uint32_t *count, int32_t *buffer);

bool msgpack_rx_handler_get_binary_array(cmp_ctx_t *ctx,char *field_name, uint32_t *count,uint32_t *binary_length,
                                         char   **binary_buffer);

bool msgpack_rx_handler_get_bin_data_ptr(cmp_ctx_t *ctx ,void **data, uint32_t *size );

bool msgpack_rx_handler_skip_field(cmp_ctx_t *ctx);

bool  msgpack_rx_handler_scoop_field(cmp_ctx_t *ctx, void **data, uint32_t *size);




#endif