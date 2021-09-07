/** @file wifi_scan.c * 
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

    lr1110_display_trx_version(&lr1110);

    struct wifi_settings wifi_settings = lr1110_get_default_wifi_settings();

    lr1110_init_wifi_scan(&lr1110);

    //lr1110_wifi_basic_complete_result_t results[LR1110_WIFI_MAX_RESULTS] = {0};
    lr1110_wifi_extended_full_result_t results[32] = {0};

    while(1)
    {
        struct wifi_diagnostics wifi_diagnostics = 
            lr1110_execute_wifi_scan(&lr1110, wifi_settings);

        //lr1110_get_wifi_scan_results(&lr1110, wifi_diagnostics, results);
        lr1110_get_ext_wifi_scan_results(&lr1110, wifi_diagnostics, results);
        //lr1110_print_wifi_scan_results(&lr1110, wifi_diagnostics, results);
        lr1110_print_ext_wifi_scan_results(&lr1110, wifi_diagnostics, results);

        k_sleep(K_MSEC(1000));
    }
}
