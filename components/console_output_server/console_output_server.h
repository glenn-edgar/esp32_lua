
#ifndef _CONSOLE_OUTPUT_SERVER_H_
#define _CONSOLE_OUTPUT_SERVER_H_

#include <stdarg.h>
#include "msg_dict_stream.h"

bool console_output_structured_data(int number, MSG_PACK_ELEMENT *msg_pack);
bool console_output_binary_data(char *prefix, char *binary_data,int binary_size);

bool console_output_server(void);
bool console_output(const char *format, ...);
#endif