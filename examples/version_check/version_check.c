/** @file version_check.c * 
 * @brief Checks version of transceiver firmware
 *
 * @par       
 * COPYRIGHT NOTICE: (c) 2021 Irnas.  All rights reserved.
 */ 

#include <zephyr.h>
#include "lr1110.h"


lr1110_t lr1110;

int main()
{
	printk("Hello World! %s\n", CONFIG_BOARD);

    lr1110_set_device_config(&lr1110, DEVICE_BOARD);
    lr1110_init(&lr1110);

	printk("Hello World! %s\n", CONFIG_BOARD);

    lr1110_system_version_t lr1110_version;
    lr1110_get_trx_version(&lr1110, &lr1110_version);
    lr1110_display_trx_version(lr1110_version);

    lr1110_init_wifi_scan(&lr1110);

    while(1)
    {
        lr1110_execute_wifi_scan(&lr1110);
        k_sleep(K_MSEC(1000));
    }
}
