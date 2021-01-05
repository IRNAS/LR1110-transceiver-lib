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

void lr1110_execute_wifi_scan(void * context)
{
   // Scan wifi
    // Scan for all types of wifi
    lr1110_wifi_signal_type_scan_t signal_type = LR1110_WIFI_TYPE_SCAN_B_G_N; //change
    // Scan all channels
    lr1110_wifi_channel_mask_t channels = DEMO_WIFI_CHANNELS_DEFAULT;    
    // Wifi capture scan mode
    lr1110_wifi_mode_t scan_mode = LR1110_WIFI_SCAN_MODE_BEACON_AND_PKT;
    //Max number of results, less or equal to 32
    uint8_t max_results = 32; //LR1110_WIFI_MAX_RESULTS;
    // Number of passive Wi-fi scans to be executed per chanell, 1 to 255
    uint8_t nb_scan_per_channel = 20;
    // Timeout in ms
    uint16_t timeout_in_ms = 100;
    // Abort on timeout, if true, then if above timeout is reached, 
    // we jump to next channel
    bool abort_on_timeout = true;

    // Mark time before starting scan and before finishing
    uint32_t start_scan = k_uptime_get();

    lr1110_wifi_scan(context, signal_type, channels, scan_mode, max_results, 
            nb_scan_per_channel, timeout_in_ms, abort_on_timeout);

    uint32_t end_scan = k_uptime_get();

    printk("Duration of wifi scan: %d\n", end_scan - start_scan);

    // Get number of results
    uint8_t nb_results = 0;
    start_scan = k_uptime_get();
    lr1110_wifi_get_nb_results(context, &nb_results);
    end_scan = k_uptime_get();

    printk("Number of wifi results: %d\n", nb_results);
    printk("Duration of getting results: %d ms\n", end_scan - start_scan);

    // Get results
    // Set index from where to read, 0-31
    uint8_t start_result_index = 0;

    lr1110_wifi_basic_complete_result_t results[LR1110_WIFI_MAX_RESULTS] = {0};

    lr1110_wifi_read_basic_complete_results(context, 
                                            start_result_index,
                                            nb_results,  
                                            results);

    // Print results
    for (int i = 0; i < nb_results; i++)
    {
        printk("{\n\t\"macAddress\": \"");
        for (int j = 0; j < LR1110_WIFI_MAC_ADDRESS_LENGTH; j++)
        {
            printk(" %02x:", results[i].mac_address[j]);
        }
        printk("\",\n\t\"signalStrength\": ");
        printk("%d\n},\n", results[i].rssi);
    }

    //this->ExecuteScan( this->device->GetRadio( ) );

    //lr1110_wifi_scan(context, DemoTransceiverWifiInterface::transceiver_wifi_scan_type_from_demo( this->settings.types ),
    //                  this->settings.channels,
    //                  DemoTransceiverWifiInterface::transceiver_wifi_mode_from_demo( this->settings.scan_mode ),
    //                  this->settings.max_results, this->settings.nbr_retrials, this->settings.timeout,
    //                  WIFI_SCAN_ABORT_ON_TIMEOUT );



    //    //wait for scan to finish
    //    
    //    this->FetchAndSaveResults( this->device->GetRadio( ) );

    //lr1110_wifi_cumulative_timings_t wifi_results_timings = { 0 };

    //lr1110_wifi_read_cumulative_timing(context, &wifi_results_timings );
}

/* -------------------------------------------------------------------------
 * PRIVATE IMPLEMENTATIONS
 * ------------------------------------------------------------------------- */

/*** end of file ***/
