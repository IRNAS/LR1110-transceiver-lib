/** @file lr1110_wifi_scan.h
 * 
 * @brief       Module containing various function wrappers for wifi scanning.
 *      
 *
 * @par       
 * COPYRIGHT NOTICE: (c) 2021 Irnas.  All rights reserved.
 */ 

#ifndef LR1110_WIFI_SCAN_H
#define LR1110_WIFI_SCAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr.h>
#include "lr1110_driver/lr1110_wifi.h"
#include "lr1110_driver/lr1110_wifi_types.h"


struct wifi_settings
{
    lr1110_wifi_signal_type_scan_t signal_type;
    lr1110_wifi_channel_mask_t channels;     
    lr1110_wifi_mode_t scan_mode;
    uint8_t max_results;
    uint8_t nb_scan_per_channel;
    uint16_t timeout_in_ms;
    bool abort_on_timeout;
};

struct wifi_diagnostics {
    uint32_t wifi_scan_duration;
    uint32_t result_fetch_duration;
    uint8_t num_wifi_results;
};

void lr1110_init_wifi_scan(void * context);
struct wifi_diagnostics
lr1110_execute_wifi_scan(void * context, struct wifi_settings wifi_settings);
struct wifi_settings lr1110_get_default_wifi_settings();
void 
lr1110_get_wifi_scan_results(void * context, 
                             struct wifi_diagnostics wifi_diagnostics,
                             lr1110_wifi_basic_complete_result_t * results);
void 
lr1110_print_wifi_scan_results(void * contex,
                               struct wifi_diagnostics wifi_diagnostics,
                               lr1110_wifi_basic_complete_result_t * results);


#ifdef __cplusplus
}
#endif

#endif /* LR1110_WIFI_SCAN_H */
/*** end of file ***/
