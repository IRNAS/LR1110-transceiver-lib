/** @file lr1110.c
 * 
 * @brief       Main lr1110.c file that calls all other modules. 
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

    if (lr1110_system_set_tcxo_mode(context, 
                                    LR1110_SYSTEM_TCXO_CTRL_3_0V, 
                                    500)){
        printk("Setting tcxo failed\n");
    }

    lr1110_system_cfg_lfclk(context, LR1110_SYSTEM_LFCLK_XTAL, true);
    lr1110_system_clear_errors(context);
    lr1110_system_calibrate(context, 0x3F); /* Value from Semtech's examples */

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


void lr1110_display_trx_version(const void * context)
{
    lr1110_system_version_t lr1110_version;
    lr1110_get_trx_version(context, &lr1110_version);

    printk("**************************************************************\n");
    printk("*                           VERSION                          *\n");
    printk("**************************************************************\n");
    printk("HARDWARE : 0x%02X\n",    lr1110_version.hw);
    printk("TYPE     : 0x%02X\n",    lr1110_version.type);
    printk("FIRMWARE : 0x%04X\n\n",  lr1110_version.fw);
}

void lr1110_set_device_config(void * context, const char * device)
{
    lr1110_set_config(context, device);
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

/* -------------------------------------------------------------------------
 * PRIVATE IMPLEMENTATIONS
 * ------------------------------------------------------------------------- */

/*** end of file ***/
