/** @file lr1110_wifi_scan.c
 * 
 * @brief       Module containing various function wrappers for wifi scanning.
 *      
 *
 * @par       
 * COPYRIGHT NOTICE: (c) 2021 Irnas.  All rights reserved.
 */ 

#include "lr1110_wifi_scan.h"
#include "lr1110.h"

/* -------------------------------------------------------------------------
 * PRIVATE VARIABLES
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * PRIVATE PROTOTYPES
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * PUBLIC IMPLEMENTATIONS
 * ------------------------------------------------------------------------- */
void lr1110_init_wifi_scan(void * context)
{
    lr1110_wifi_reset_cumulative_timing(context);
    lr1110_wifi_cfg_hardware_debarker(context, true);
}

#define DEMO_WIFI_CHANNELS_DEFAULT                                       \
    ( ( 1 << LR1110_WIFI_CHANNEL_11 ) + ( 1 << LR1110_WIFI_CHANNEL_6 ) + \
      ( 1 << LR1110_WIFI_CHANNEL_1 ) )

struct wifi_settings lr1110_get_default_wifi_settings()
{
    struct wifi_settings wifi_settings = {
        .signal_type            = LR1110_WIFI_TYPE_SCAN_B_G_N,
        .channels               = LR1110_WIFI_ALL_CHANNELS,
        .scan_mode              = LR1110_WIFI_SCAN_MODE_BEACON_AND_PKT,
        .max_results            = LR1110_WIFI_MAX_RESULTS,
        .nb_scan_per_channel    = 30,
        .timeout_in_ms          = 110,
        .abort_on_timeout       = false,
    };
    return wifi_settings;
}


struct wifi_diagnostics
lr1110_execute_wifi_scan(void * context, struct wifi_settings wifi_settings)
{
    struct wifi_diagnostics wifi_diagnostics = {0};

    /* Prepare event intterupt line */
    lr1110_prepare_event(context, LR1110_SYSTEM_IRQ_WIFI_SCAN_DONE);

    uint32_t start_scan = k_uptime_get();

    /* Start up wifi scan, function itself is not blocking, 
     * however, we wait for WIFI_SCAN_DONE event, 
     * as it currently does not make sense to implement interrupt. */
    lr1110_wifi_scan(context, 
                     wifi_settings.signal_type, 
                     wifi_settings.channels, 
                     wifi_settings.scan_mode, 
                     wifi_settings.max_results, 
                     wifi_settings.nb_scan_per_channel, 
                     wifi_settings.timeout_in_ms, 
                     wifi_settings.abort_on_timeout);

    /* Blocking wait */
    lr1110_wait_for_event(context);
    uint32_t end_scan = k_uptime_get();

    wifi_diagnostics.wifi_scan_duration = end_scan - start_scan;

    /* Clear event interrupt line */
    lr1110_clear_event(context, LR1110_SYSTEM_IRQ_WIFI_SCAN_DONE);

    /* Get number of wifi scan results */
    start_scan = k_uptime_get();
    lr1110_wifi_get_nb_results(context, &wifi_diagnostics.num_wifi_results);
    end_scan = k_uptime_get();

    wifi_diagnostics.result_fetch_duration = end_scan - start_scan;
    return wifi_diagnostics;
}


void 
lr1110_get_wifi_scan_results(void * context, 
                             struct wifi_diagnostics wifi_diagnostics,
                             lr1110_wifi_basic_complete_result_t * results)
{
    lr1110_wifi_read_basic_complete_results(context, 
                                            0,  /* start result index */
                                            wifi_diagnostics.num_wifi_results,
                                            results);
}


void 
lr1110_print_wifi_scan_results(void * contex,
                               struct wifi_diagnostics wifi_diagnostics,
                              lr1110_wifi_basic_complete_result_t * results)
{

    printk("**************************************************************\n");
    printk("*                      WIFI SCAN RESULTS                     *\n");
    printk("**************************************************************\n");
    printk("Scan duration:          %d ms\n", 
            wifi_diagnostics.wifi_scan_duration);
    printk("Fetch result duration:  %d ms\n", 
            wifi_diagnostics.result_fetch_duration);
    printk("Number of wifi results: %d\n", 
            wifi_diagnostics.num_wifi_results);

    /* Print results*/
    for (int i = 0; i < wifi_diagnostics.num_wifi_results; i++)
    {
        printk("{\n\t\"macAddress\": \"");
        for (int j = 0; j < LR1110_WIFI_MAC_ADDRESS_LENGTH; j++)
        {
            printk(" %02x:", results[i].mac_address[j]);
        }
        printk("\",\n\t\"signalStrength\": ");
        printk("%d\n},\n", results[i].rssi);
    }
}

/* -------------------------------------------------------------------------
 * PRIVATE IMPLEMENTATIONS
 * ------------------------------------------------------------------------- */

/*** end of file ***/
