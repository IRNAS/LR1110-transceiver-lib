/** @file lr1110.h
 * 
 * @brief       Main lr1110.h file that calls all other modules. 
 *              Only this file should be included into application.
 *      
 *
 * @par       
 * COPYRIGHT NOTICE: (c) 2020 Irnas.  All rights reserved.
 */ 
 
#ifndef LR1110_TRX_H
#define LR1110_TRX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include "lr1110_driver/lr1110_system_types.h"

// TODO: Remove later Tue 05 Jan 2021 15:33:34 CET
#include "lr1110_driver/lr1110_system.h"
#include "lr1110_driver/lr1110_system_types.h"
#include "lr1110_driver/lr1110_wifi.h"
#include "lr1110_driver/lr1110_wifi_types.h"
/*!
 * @brief Struct for storing data about port and pin number for each physical
 *        pin. If using GPIO_0, port should be set to 0, if using GPIO_1 then 
 *        set to 1.
 */
typedef struct
{
    struct device * port;
    gpio_pin_t pin;
} port_pin_t;

/*!
 * @brief Radio hardware and global parameters, also known as context
 */
typedef struct
{
    port_pin_t reset;
    port_pin_t nss;
    port_pin_t event;
    port_pin_t busy;
    port_pin_t lna;
    char * spi_dev_label;
    lr1110_system_rfswitch_cfg_t * rf_switch_cfg;
    void (*event_interrupt_cb)(void);
    gpio_flags_t event_trigger_type;
} lr1110_t;


void lr1110_init(const void * context);
void lr1110_set_device_config(void * context, const char * device);

void lr1110_get_trx_version(const void * context, 
                            lr1110_system_version_t * lr1110_version);
void lr1110_display_trx_version(lr1110_system_version_t lr1110_version);


struct wifi_scan_settings
{
    lr1110_wifi_signal_type_scan_t signal_type;
    lr1110_wifi_channel_mask_t channels;     
    lr1110_wifi_mode_t scan_mode;
    uint8_t max_results;
    uint8_t nb_scan_per_channel;
    uint16_t timeout_in_ms;
    bool abort_on_timeout;
};

struct wifi_scan_dia_results {
    uint32_t wifi_scan_duration;
    uint32_t result_fetch_duration;
    uint8_t num_wifi_results;
};

void lr1110_init_wifi_scan(void * context);
struct wifi_scan_dia_results
lr1110_execute_wifi_scan(void * context, 
                         struct wifi_scan_settings wifi_scan_settings);
struct wifi_scan_settings lr1110_get_default_wifi_scan_settings(void * context);
void lr1110_prepare_event(void * context, lr1110_system_irq_mask_t event_mask);
void lr1110_wait_for_event(void * context);
void lr1110_clear_event(void * context, lr1110_system_irq_mask_t event_mask);

void lr1110_get_wifi_scan_results(void * context, 
                                  struct wifi_scan_dia_results wifi_scan_dia_results,
                                  lr1110_wifi_basic_complete_result_t * results);
void lr1110_print_wifi_scan_results(void * contex,
                                  struct wifi_scan_dia_results wifi_scan_dia_results,
                                  lr1110_wifi_basic_complete_result_t * results);





#ifdef __cplusplus
}
#endif

#endif /* LR1110_TRX_H */
/*** end of file ***/
