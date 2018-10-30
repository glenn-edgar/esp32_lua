#include <esp_types.h>
#include "hexadecimal.h"

static const char hex_array[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
    
    
    

void binary_to_hex(char *input,int input_number, char *output)
{
    int temp;
    for(int i=0;i<input_number;i++)
    {
        temp = *input;
        temp = (temp>>4)&0xf;
        *output++ = hex_array[temp];
        temp = *input++;
        temp = (temp)&0xf;
        *output++ = hex_array[temp];

   
        
    }
    
    
}

void hex_to_binary(char *input,int input_number, char *output)
{
    uint8_t result;
    for(int i= 0; i < input_number;i++)
    {
        result =0;
        switch( *input++)
        {
            case '0':
                  result = 0;
                  break;

            case '1':
                  result = 0x10;
                  break;

            case '2':
                   result = 0x20;
                   break;

            case '3':
                  result = 0x30;
                  break;

            case '4':
                  result = 0x40;
                  break;

            case '5':
                       result = 0x50;
                       break;

            case '6':
                       result = 0x60;
                       break;

            case '7':
                       result = 0x70;
                       break;

            case '8':
                       result = 0x80;
                       break;

            case '9':
                       result = 0x90;
                       break;
            case 'a':
            case 'A':
                       result = 0xa0;
                       break;
            case 'b':
            case 'B':
                       result = 0xB0;
                      break;
            case 'c':
            case 'C':
                       result = 0xc0;
                       break;
            case 'd':
            case 'D':
                       result = 0xd0;
                       break;
            case 'e':
            case 'E':
                       result = 0xe0;
                       break;
            case 'f':
            case 'F':
                       result = 0xf0;
                       break;
        }
        
         switch( *input++)
        {
            case '0':
                  
                  break;

            case '1':
                  result |= 0x1;
                  break;

            case '2':
                   result |= 0x2;
                   break;

            case '3':
                  result |= 0x3;
                  break;

            case '4':
                  result |= 0x4;
                  break;

            case '5':
                       result |= 0x5;
                       break;

            case '6':
                       result |= 0x6;
                       break;

            case '7':
                       result |= 0x7;
                       break;

            case '8':
                       result |= 0x8;
                       break;

            case '9':
                       result |= 0x9;
                       break;
            case 'a':
            case 'A':
                       result |= 0xa;
                       break;
            case 'b':
            case 'B':
                       result |= 0xB;
                       break;
            case 'c':
            case 'C':
                       result |= 0xc;
                       break;

            case 'd':
            case 'D':
                       result |= 0xd;
                       break;

            case 'e':
            case 'E':
                       result |= 0xe;
                       break;
            case 'f':
            case 'F':
                       result |= 0xf;
                       break;
        }
        *output++ = result;
               
        
        
        
        
    }
    
    
    
    
    
}