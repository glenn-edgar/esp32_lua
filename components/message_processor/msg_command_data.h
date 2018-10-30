
#ifndef _MSG_COMMAND_DATA_H_
#define _MSG_COMMAND_DATA_H_
#include "cmp.h"

#include "msg_dict_stream.h"
#define MSG_PACK_SIZE 100
#define RETURN_BUFFER_SIZE 2000
#define COMMAND_FIELD "COMMAND"
#define DATA_FIELD "DATA"


void msg_command_data_initialize(void);
bool msg_command_process_packet(int *msg_pack_number, 
                                
                                 MSG_PACK_ELEMENT **msg_pack,
                                char * input_buffer,int input_number);





#endif