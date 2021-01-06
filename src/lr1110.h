/** @file lr1110.h
 * 
 * @brief       Main lr1110.h file that calls all other modules. 
 *              Only this file should be included into application.
 *      
 *
 * @par       
 * COPYRIGHT NOTICE: (c) 2021 Irnas.  All rights reserved.
 */ 
 
#ifndef LR1110_TRX_H
#define LR1110_TRX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include "lr1110_driver/lr1110_system.h"
#include "lr1110_driver/lr1110_system_types.h"
#include "lr1110_wifi_scan.h"


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
void lr1110_display_trx_version(const void * context);

void lr1110_prepare_event(void * context, lr1110_system_irq_mask_t event_mask);
void lr1110_wait_for_event(void * context);
void lr1110_clear_event(void * context, lr1110_system_irq_mask_t event_mask);

#ifdef __cplusplus
}
#endif

#endif /* LR1110_TRX_H */
/*** end of file ***/
