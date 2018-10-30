
#include <stdio.h>
#include "driver/i2c.h"
#include "slave_i2c_interface.h"

#define WRITE_BIT                          I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                           I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                       0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                      0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                            0x0              /*!< I2C ack value */
#define NACK_VAL                           0x1              /*!< I2C nack value */


/*
Note:
To read a slave buffer 

int i2c_slave_read_buffer(i2c_port_ti2c_num, uint8_t *data, size_t max_size, TickType_t ticks_to_wait);
example 
size = i2c_slave_read_buffer( I2C_EXAMPLE_SLAVE_NUM, data, RW_TEST_LENGTH, 1000 / portTICK_RATE_MS);

To write a slave buffer

int i2c_slave_write_buffer(i2c_port_ti2c_num, uint8_t *data, int size, TickType_t ticks_to_wait);
example
size_t d_size = i2c_slave_write_buffer(I2C_EXAMPLE_SLAVE_NUM, data, RW_TEST_LENGTH, 1000 / portTICK_RATE_MS);

*/


void i2c_slave_init( int port, 
                             int sda_io, 
                             int sda_clk, 
                             int slave_address, 
                             int rx_buffer_length,
                             int tx_buffer_length)
{
    int i2c_slave_port = port;
    i2c_config_t conf_slave;
    conf_slave.sda_io_num = sda_io;
    conf_slave.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf_slave.scl_io_num = sda_clk;
    conf_slave.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf_slave.mode = I2C_MODE_SLAVE;
    conf_slave.slave.addr_10bit_en = 0;
    conf_slave.slave.slave_addr = slave_address;
    i2c_param_config(i2c_slave_port, &conf_slave);
    i2c_driver_install(i2c_slave_port, conf_slave.mode,
                       rx_buffer_length,
                       tx_buffer_length, 0);
}