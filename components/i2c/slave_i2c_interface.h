

#ifndef _SLAVE_I2C_INTERFACE_H_
#define _SLAVE_I2C_INTERFACE_H_



void i2c_slave_init( int port, 
                             int sda_io, 
                             int sda_clk, 
                             int slave_address, 
                             int rx_buffer_length,
                             int tx_buffer_length);
                             
                             
#endif