

#ifndef _MESSAGE_PROCESSOR_
#define _MESSAGE_PROCESSOR_
#include "msg_dict_stream.h"

void init_message_processor(void);
bool message_processor(int *msg_pack_number, 
                       MSG_PACK_ELEMENT **msg_pack,
                       char * input_buffer,
                       int input_number);

#endif