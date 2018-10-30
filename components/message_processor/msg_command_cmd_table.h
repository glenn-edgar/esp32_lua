
#ifndef _MSG_COMMAND_CMD_TABLE_H_
#define _MSG_COMMAND_CMD_TABLE_H_

#include "cmp.h"
#include "msg_dict_stream.h"
#define COMMAND_TABLE_NUMBER 64


typedef bool (*PROCESS_COMMAND_T)( cmp_ctx_t *ctx,
                                   int *msg_pack_number, 
                                   MSG_PACK_ELEMENT **msg_pack); 
 
void initialize_command_table( void );

void msg_command_add_command(char *command_name,PROCESS_COMMAND_T process_command);

PROCESS_COMMAND_T msg_command_find_command(char *command_name, uint32_t command_size);

int msg_command_command_number(void);

char *msg_command_list_cmd_name(int index);
#endif
    