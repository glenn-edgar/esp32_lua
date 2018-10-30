
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "msgpack_rx_handler.h"
#include "msg_command_cmd_table.h"


typedef struct
{
    char *command_name;
    PROCESS_COMMAND_T process_command;
}COMMAND_TABLE_T;

static COMMAND_TABLE_T command_table[COMMAND_TABLE_NUMBER+1];

static int command_number_size;

void initialize_command_table( void )
{
    
    command_number_size = 0;
    memset(command_table,0,sizeof(command_table));
    
}

int msg_command_command_number(void)
{
    return command_number_size-1;
    
}


char *msg_command_list_cmd_name(int index)
{
    char *return_value;
    if( index >= command_number_size )
    {
          return_value = NULL;
        
    }
    else
    {
        return_value = command_table[index].command_name;
    }
    return return_value;
    
}

void msg_command_add_command(char *command_name,PROCESS_COMMAND_T process_command)
{
    char *stored_command_name;
    
    command_number_size +=1;
    
    stored_command_name = malloc(strlen(command_name)+5);
    memset(stored_command_name,0,strlen(command_name)+5);
    if(stored_command_name == NULL)
    {
        abort();
    }
    strcpy(stored_command_name,command_name);
    
    for(int i=0;i<COMMAND_TABLE_NUMBER;i++)
    {
        
        if( command_table[i].command_name == NULL )
        {   
            command_table[i].command_name =  stored_command_name;
            command_table[i].process_command = process_command;
            return;
        }    
            
    }
    abort();   
        
}
    
    
    
PROCESS_COMMAND_T msg_command_find_command(char *command_name, uint32_t command_size)
{
    PROCESS_COMMAND_T return_value;
    return_value = NULL;
    for(int i=0;i<COMMAND_TABLE_NUMBER;i++)
    {
        if(command_table[i].command_name == NULL)
        {
            break;
        }
       
        if( ctx_strcmp( command_table[i].command_name, command_name,command_size) == true )
        {
            
            return_value = command_table[i].process_command;
           
        }    
            
    }
    
    return return_value;
}
    
    
    
    
 