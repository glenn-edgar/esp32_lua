#ifndef _MASTER_I2C_INTERFACE_H_
#define _MASTER_I2C_INTERFACE_H_


void i2c_master_init(int port,int sta_io,int scl_io,int clock_speed );

esp_err_t i2c_master_read_slave(i2c_port_t i2c_num,
                                               int slave_address, 
                                               uint8_t* data_rd, 
                                               size_t size);
                                               
                                               
esp_err_t i2c_master_write_slave(i2c_port_t i2c_num,
                                                int slave_address, 
                                                uint8_t* data_wr, 
                                                size_t size);    
          
#endif