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
    lr1110_hal_operating_mode_t op_mode;
    lr1110_modem_system_rf_switch_cfg_t * rf_switch_cfg;
    void (*event_interrupt_cb)(void);
    gpio_flags_t event_trigger_type;
} lr1110_t;


void lr1110_init(const void * context);
void lr1110_set_device_config(const void * context, const char * device);

#ifdef __cplusplus
}
#endif

#endif /* LR1110_TRX_H */
/*** end of file ***/