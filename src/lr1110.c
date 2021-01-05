/** @file lr1110.c
 * 
 * @brief       Main lr1110.h file that calls all other modules. 
 *              Only this file should be included into application.
 *      
 *
 * @par       
 * COPYRIGHT NOTICE: (c) 2021 Irnas.  All rights reserved.
 */ 

#include "lr1110.h"
#include "lr1110_configs.h"
#include "lr1110_trx_board.h"
#include "lr1110_driver/lr1110_hal.h"
#include "lr1110_driver/lr1110_system_types.h"
#include "lr1110_driver/lr1110_system.h"
#include "lr1110_driver/lr1110_bootloader.h"
#include "lr1110_driver/lr1110_wifi.h"
#include "lr1110_driver/lr1110_wifi_types.h"

/* -------------------------------------------------------------------------
 * PRIVATE VARIABLES
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * PRIVATE PROTOTYPES
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * PUBLIC IMPLEMENTATIONS
 * ------------------------------------------------------------------------- */


void lr1110_init(const void * context)
{
    lr1110_gpio_init(context);
    lr1110_spi_init(context);

    lr1110_hal_reset(context);

    /* Added to let the radio perform it startup sequence */
    k_sleep(K_MSEC(500));

    if (lr1110_system_set_reg_mode(context, LR1110_SYSTEM_REG_MODE_DCDC)) {
        printk("Setting DCDC mode failed\n");
    }

    if (lr1110_rf_switch_init(context)) {
        printk("RF switch init failed\n");
    }

    if (lr1110_system_set_tcxo_mode(context, LR1110_SYSTEM_TCXO_CTRL_3_0V, 500)){
        printk("Setting tcxo failed\n");

    }
    lr1110_system_cfg_lfclk(context, LR1110_SYSTEM_LFCLK_XTAL, true);
    lr1110_system_clear_errors(context);
    lr1110_system_calibrate(context, 0x3F);

    uint16_t errors;
    lr1110_system_get_errors(context, &errors);
    lr1110_system_clear_errors(context);
    lr1110_system_clear_irq_status(context, LR1110_SYSTEM_IRQ_ALL_MASK);
}


void lr1110_get_trx_version(const void * context, 
                            lr1110_system_version_t * lr1110_version)
{
    if (lr1110_system_get_version(context, lr1110_version)) {
        printk("Getting system version failed\n");
    }
}


void lr1110_display_trx_version(lr1110_system_version_t lr1110_version)
{
    printk("HARDWARE : 0x%02X\n",    lr1110_version.hw);
    printk("TYPE     : 0x%02X\n",    lr1110_version.type);
    printk("FIRMWARE : 0x%04X\n\n",  lr1110_version.fw);
}

void lr1110_set_device_config(void * context, const char * device)
{
    lr1110_set_config(context, device);
}

void lr1110_init_wifi_scan(void * context)
{
    lr1110_wifi_reset_cumulative_timing(context);
    lr1110_wifi_cfg_hardware_debarker(context, true);
}

#define DEMO_WIFI_CHANNELS_DEFAULT                                       \
    ( ( 1 << LR1110_WIFI_CHANNEL_11 ) + ( 1 << LR1110_WIFI_CHANNEL_6 ) + \
      ( 1 << LR1110_WIFI_CHANNEL_1 ) )

struct wifi_scan_settings lr1110_get_default_wifi_scan_settings(void * context)
{
    struct wifi_scan_settings wifi_scan_settings = {
        .signal_type = LR1110_WIFI_TYPE_SCAN_B_G_N,
        .channels = LR1110_WIFI_ALL_CHANNELS,
        .scan_mode = LR1110_WIFI_SCAN_MODE_BEACON_AND_PKT,
        .max_results = LR1110_WIFI_MAX_RESULTS,
        .nb_scan_per_channel = 20,
        .timeout_in_ms = 100,
        .abort_on_timeout = true,
    };
    return wifi_scan_settings;
}

void lr1110_prepare_event(void * context, lr1110_system_irq_mask_t event_mask)
{
    lr1110_system_set_dio_irq_params(context, event_mask, 0);
}

void lr1110_wait_for_event(void * context)
{
	while (0 == gpio_pin_get(((lr1110_t*) context)->event.port, 
                             ((lr1110_t*) context)->event.pin));
}

void lr1110_clear_event(void * context, lr1110_system_irq_mask_t event_mask)
{
    lr1110_system_clear_irq_status(context,  event_mask);
}

struct wifi_scan_dia_results
lr1110_execute_wifi_scan(void * context, 
                         struct wifi_scan_settings wifi_scan_settings)
{
    struct wifi_scan_dia_results wifi_scan_dia_results = {0};

    /* Prepare event intterupt line*/
    lr1110_prepare_event(context, LR1110_SYSTEM_IRQ_WIFI_SCAN_DONE);

    uint32_t start_scan = k_uptime_get();

    lr1110_wifi_scan(context, 
                     wifi_scan_settings.signal_type, 
                     wifi_scan_settings.channels, 
                     wifi_scan_settings.scan_mode, 
                     wifi_scan_settings.max_results, 
                     wifi_scan_settings.nb_scan_per_channel, 
                     wifi_scan_settings.timeout_in_ms, 
                     wifi_scan_settings.abort_on_timeout);

    lr1110_wait_for_event(context);
    uint32_t end_scan = k_uptime_get();

    wifi_scan_dia_results.wifi_scan_duration = end_scan - start_scan;

    /* Clear event intterupt line*/
    lr1110_clear_event(context, LR1110_SYSTEM_IRQ_WIFI_SCAN_DONE);

    start_scan = k_uptime_get();
    lr1110_wifi_get_nb_results(context, 
                               &wifi_scan_dia_results.num_wifi_results);
    end_scan = k_uptime_get();

    wifi_scan_dia_results.result_fetch_duration = end_scan - start_scan;
    return wifi_scan_dia_results;
}

void lr1110_get_wifi_scan_results(void * context, 
                                  struct wifi_scan_dia_results wifi_scan_dia_results,
                                  lr1110_wifi_basic_complete_result_t * results)
{
    lr1110_wifi_read_basic_complete_results(context, 
                                            0,  /* start result index */
                                            wifi_scan_dia_results.num_wifi_results,
                                            results);
}

void lr1110_print_wifi_scan_results(void * contex,
                                  struct wifi_scan_dia_results wifi_scan_dia_results,
                                  lr1110_wifi_basic_complete_result_t * results)
{

    printk("--------------------------------------------------------------\n");
    printk("Scan duration:          %d ms\n", 
            wifi_scan_dia_results.wifi_scan_duration);
    printk("Fetch result duration:  %d ms\n", 
            wifi_scan_dia_results.result_fetch_duration);
    printk("Number of wifi results: %d\n", 
            wifi_scan_dia_results.num_wifi_results);

    /* Print results*/
    for (int i = 0; i < wifi_scan_dia_results.num_wifi_results; i++)
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
