// Copyright (c) 2017 Olimex Ltd.
//
// GNU GENERAL PUBLIC LICENSE
//    Version 3, 29 June 2007
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_err.h"
#include "esp_event_loop.h"
#include "esp_event.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_eth.h"

#include "rom/ets_sys.h"
#include "rom/gpio.h"

#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"

#include "tcpip_adapter.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "msgpack_rx_handler.h"

// #include "eth_phy/phy_lan8720.h"
#include "olimex_lan8710.h"
#include "olimex_ethernet.h"

#define PIN_SMI_MDC   23
#define PIN_SMI_MDIO  18

char hostname[32];


#define CHECK_ERROR_CODE(returned, expected) ({                        \
            if(returned != expected){                                  \
                printf("ETHERNET INITIALIZATION PROBLEM\n");                                \
                abort();                                               \
            }                                                          \
})

static bool find_host_name(void);

static void eth_gpio_config_rmii(void)
{
    // RMII data pins are fixed:
    // TXD0 = GPIO19
    // TXD1 = GPIO22
    // TX_EN = GPIO21
    // RXD0 = GPIO25
    // RXD1 = GPIO26
    // CLK == GPIO0
    phy_rmii_configure_data_interface_pins();
    // MDC is GPIO 23, MDIO is GPIO 18
    phy_rmii_smi_configure_pins(PIN_SMI_MDC, PIN_SMI_MDIO);
}

void initEthernet(void)
{
    printf("Initialize ETHERNET\n"); 
    
   
    if( find_host_name() == false){ return; } // no host name

    eth_config_t config = phy_lan8710_default_ethernet_config;
  

    /* Initialize adapter */
    tcpip_adapter_init();
  
    esp_event_loop_init(NULL, NULL);
   
    /* Set the PHY address in the example configuration */
    config.phy_addr = 0;
    config.gpio_config = eth_gpio_config_rmii;
    config.tcpip_input = tcpip_adapter_eth_input;
    
    /* Initialize ethernet */
    CHECK_ERROR_CODE(esp_eth_init(&config), ESP_OK);
    
    /* Enable ethernet */
    
    CHECK_ERROR_CODE(esp_eth_enable(), ESP_OK);
    tcpip_adapter_set_hostname(ESP_IF_ETH, hostname);

}

static bool find_host_name(void)
{
    bool ret;
    char     *buffer;
    uint32_t  buffer_size;
    
    cmp_ctx_t ctx;
    
    buffer_size = 256;
    ret = msgpack_rx_handler_file(&ctx,"/spiffs/WIFI.MPK",&buffer,&buffer_size );
    if(ret == false)
    {
        return false;
    }
 
    buffer_size = sizeof(hostname);
    ret = msgpack_rx_handler_find_string( &ctx,"hostname", hostname, &buffer_size );


    

    free(buffer);
    return ret;
}


