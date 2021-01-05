/** @file lr1110_trx_board.c
 * 
 * @brief This module deals with GPIO and SPI initializations in connection
 *        with lr1110 chip. It implements functions declared in lr1110_hal.h, 
 *        as well in lr1110-modem-board.h
 *
 * @par       
 * COPYRIGHT NOTICE: (c) 2021 Irnas. All rights reserved.
 */ 

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>

#include "lr1110_trx_board.h"
#include "lr1110.h"
#include "lr1110_driver/lr1110_hal.h"
#include "lr1110_driver/lr1110_system.h"
#include "lr1110_driver/lr1110_system_types.h"


/* -------------------------------------------------------------------------
 * PRIVATE VARIABLES
 * ------------------------------------------------------------------------- */
static const struct device * spi_dev;
static struct spi_config spi_cfg;


/* -------------------------------------------------------------------------
 * PRIVATE PROTOTYPES
 * ------------------------------------------------------------------------- */
static void lr1110_set_nss(const void * context, uint8_t level);
static lr1110_hal_status_t lr1110_hal_wait_busy(const void * context, 
                                                uint32_t timeout_ms);
static void lr1110_spi_write(const void * context,
                             const uint8_t * command, 
                             const uint16_t command_length);
static void lr1110_spi_read(const void * context,
                            uint8_t * data, 
                            uint16_t data_length);


//static void lr1110_spi_read(const void * context,
//                            uint8_t *command, 
//                            uint8_t size_command,
//                            uint8_t * data, 
//                            uint16_t data_length);


static void lr1110_spi_write_read(const void * context, 
                                  const uint8_t * command, 
                                  uint8_t * data,
                                  const uint16_t data_length);
static lr1110_system_rfswitch_cfg_t create_evk_shield_rf_switch();

lr1110_hal_status_t lr1110_hal_wakeup(const void * context);
/* -------------------------------------------------------------------------
 * PUBLIC IMPLEMENTATIONS
 * ------------------------------------------------------------------------- */

/*!
 * @brief               Initializes GPIO peripherals for LR1110 
 *
 * @param[in] context   Radio abstraction
 */
void lr1110_gpio_init(const void * context)
{
    /*  Nss pin, output, it is needed here because of lr1110_hal_wakeup */
    gpio_pin_configure(((lr1110_t*) context)->nss.port, 
                       ((lr1110_t*) context)->nss.pin,
                       GPIO_OUTPUT_HIGH);
    /* Reset pin, output */
	gpio_pin_configure(((lr1110_t*) context)->reset.port, 
                       ((lr1110_t*) context)->reset.pin,
                       GPIO_OUTPUT_HIGH);
    /* Busy pin, input */
	gpio_pin_configure(((lr1110_t*) context)->busy.port, 
                       ((lr1110_t*) context)->busy.pin,
                       GPIO_INPUT);
    /* LNA pin, output */
	gpio_pin_configure(((lr1110_t*) context)->lna.port, 
                       ((lr1110_t*) context)->lna.pin,
                       GPIO_OUTPUT_LOW);

	gpio_pin_configure(((lr1110_t*) context)->event.port, 
                       ((lr1110_t*) context)->event.pin,
                       GPIO_INPUT);
}


/*!
 * @brief               Initializes SPI peripheral for LR1110 
 *
 * @param[in] context   Radio abstraction
 * @note                Nss pin has to be controlled manually, as automatic 
 *                      control thourgh spi_cs_ctrl does not work
 */
void lr1110_spi_init(const void * context)
{
    spi_dev = device_get_binding(((lr1110_t*) context)->spi_dev_label);

    if (!spi_dev){
        printk("spi device not found: %s\n", 
                ((lr1110_t*) context)->spi_dev_label);
    }

    spi_cfg.operation = (SPI_OP_MODE_MASTER | 
                         SPI_TRANSFER_MSB | 
                         SPI_WORD_SET(8));

    spi_cfg.frequency = 4000000;
    spi_cfg.slave = 0;
}


/*!
 * @brief                       HAL write
 *
 * @param[in] context           Radio abstraction
 * @param[in] command           Pointer to the buffer to be transmitted
 * @param[in] command_length    Buffer size to be transmitted
 * @param[out] data             Pointer to the buffer to be transmitted
 * @param[in] data_length       Buffer size to be transmitted
 *
 * @return status               HAL status
 */
lr1110_hal_status_t lr1110_hal_write(const void * context, 
                                     const uint8_t * command, 
                                     const uint16_t command_length, 
                                     const uint8_t * data, 
                                     const uint16_t data_length)
{
    if(lr1110_hal_wait_busy(context, 2000)) {
        return LR1110_HAL_STATUS_ERROR;
    }

    lr1110_set_nss(context, 0);
    lr1110_spi_write(context, command, command_length);
    lr1110_spi_write(context, data, data_length);
    lr1110_set_nss(context, 1);

    return LR1110_HAL_STATUS_OK;
}


/*!
 * @brief                       HAL read
 *
 * @param[in] context           Radio abstraction
 * @param[in] command           Pointer to the buffer to be transmitted
 * @param[in] command_length    Buffer size to be transmitted
 * @param[out] data             Pointer to the buffer to be received
 * @param[in] data_length       Buffer size to be received
 *
 * @return status               HAL status
 */
lr1110_hal_status_t lr1110_hal_read(const void * context, 
                                    const uint8_t * command, 
                                    const uint16_t command_length, 
                                    uint8_t * data, 
                                    const uint16_t data_length)
{
    uint8_t dummy = 0x00;

    if(lr1110_hal_wait_busy(context, 2000)) {
        return LR1110_HAL_STATUS_ERROR;
    }

    /* 1st SPI transaction */
    lr1110_set_nss(context, 0);
    lr1110_spi_write(context, command, command_length);
    lr1110_set_nss(context, 1);

    if(lr1110_hal_wait_busy(context, 2000)) {
        return LR1110_HAL_STATUS_ERROR;
    }

    /* 2nd SPI transaction */
    lr1110_set_nss(context, 0);
    lr1110_spi_write(context, &dummy, 1);
    lr1110_spi_read(context, data, data_length);
    lr1110_set_nss(context, 1);

    return LR1110_HAL_STATUS_OK;
}


/*!
 * @brief                       HAL write read
 *
 * @param[in] context           Radio abstraction
 * @param[in] command           Pointer to the buffer to be transmitted
 * @param[out] data             Pointer to the buffer to be received
 * @param[in] data_length       Size of both buffers
 *
 * @return status               HAL status
 */
lr1110_hal_status_t lr1110_hal_write_read(const void * context, 
                                          const uint8_t * command, 
                                          uint8_t * data, 
                                          const uint16_t data_length)
{
    if(lr1110_hal_wait_busy(context, 2000)) {
        return LR1110_HAL_STATUS_ERROR;
    }

    lr1110_set_nss(context, 0);
    lr1110_spi_write_read(context, command, data, data_length);
    lr1110_set_nss(context, 1);

    return LR1110_HAL_STATUS_OK;
}


/*!
 * @brief               Configures RF switches
 *
 * @param[in] context   Radio abstraction
 */
lr1110_status_t lr1110_rf_switch_init(const void * context)
{
    lr1110_system_rfswitch_cfg_t rf_switch_cfg;

    if (((lr1110_t*) context)->rf_switch_cfg != NULL) {
        /* Custom rf switch configuration is used */
        rf_switch_cfg = *(((lr1110_t*) context)->rf_switch_cfg);
    }
    else {
        /* Default, EVK shield configuration is used */
        rf_switch_cfg = create_evk_shield_rf_switch();
    }
    
    return lr1110_system_set_dio_as_rf_switch(context, &rf_switch_cfg);
}


/*!
 * @brief               Function returns rf switch configuration for 
 *                      LR1110 EVK shield
 *
 * @return rf_switch_configuration struct
 */
static lr1110_system_rfswitch_cfg_t create_evk_shield_rf_switch()
{
    lr1110_system_rfswitch_cfg_t rf_switch_configuration;

    rf_switch_configuration.enable  = LR1110_SYSTEM_RFSW0_HIGH | 
                                      LR1110_SYSTEM_RFSW1_HIGH | 
                                      LR1110_SYSTEM_RFSW2_HIGH;
    rf_switch_configuration.standby = 0;

    /* LoRa SPDT */
    rf_switch_configuration.rx = LR1110_SYSTEM_RFSW0_HIGH;
    rf_switch_configuration.tx = LR1110_SYSTEM_RFSW0_HIGH | 
                                 LR1110_SYSTEM_RFSW1_HIGH;

    rf_switch_configuration.tx_hp = LR1110_SYSTEM_RFSW1_HIGH;

	/* GNSS LNA ON */
    rf_switch_configuration.gnss = LR1110_SYSTEM_RFSW2_HIGH;

    rf_switch_configuration.tx_hf = 0x00;
    rf_switch_configuration.wifi = 0x00;

    return rf_switch_configuration;
}


/* -------------------------------------------------------------------------
 * PRIVATE IMPLEMENTATIONS
 * ------------------------------------------------------------------------- */


/*!
 * @brief                       Set slave select line
 *
 * @param[in] context           Radio abstraction
 * @param[in] level             Logical level of slave select line
 */
static void lr1110_set_nss(const void * context, uint8_t level)
{
	gpio_pin_set(((lr1110_t*) context)->nss.port, 
                 ((lr1110_t*) context)->nss.pin, level);
}


/*!
 * @brief                       Perform reset of LR1110 chip
 *
 * @param[in] context           Radio abstraction
 */
lr1110_hal_status_t lr1110_hal_reset(const void * context)
{
	gpio_pin_set(((lr1110_t*) context)->reset.port, 
                 ((lr1110_t*) context)->reset.pin, 0);
    k_sleep(K_MSEC(500));
	gpio_pin_set(((lr1110_t*) context)->reset.port, 
                 ((lr1110_t*) context)->reset.pin, 1);

    return LR1110_HAL_STATUS_OK;
}


/*!
 * @brief                       Perform wakeup sequence of LR1110 chip
 *
 * @param[in] context           Radio abstraction
 *
 * @return status               HAL status
 */
lr1110_hal_status_t lr1110_hal_wakeup(const void * context)
{
    lr1110_set_nss(context, 0);
    k_sleep(K_MSEC(1));
    lr1110_set_nss(context, 1);

    return LR1110_HAL_STATUS_OK;
}

/*!
 * @brief                       Perform wakeup sequence of LR1110 chip
 *
 * @param[in] context           Radio abstraction
 * @param[in] timeout_ms        Timeout in milliseconds
 *
 * @return status               HAL status
 */
static lr1110_hal_status_t lr1110_hal_wait_busy(const void * context, 
                                                uint32_t timeout_ms)
{
    uint32_t start = k_uptime_get();

    /* Wait while busy is HIGH */
	while (1 == gpio_pin_get(((lr1110_t*) context)->busy.port, 
                             ((lr1110_t*) context)->busy.pin))
    {
        if ((k_uptime_get() - start) > timeout_ms)
        {
            printk("------------------------------------------------------\n");
            printk("WAIT BUSY TIMEOUTED\n");
            printk("THIS SHOULD NOT HAPPEN\n");
            printk("------------------------------------------------------\n");
            return LR1110_HAL_STATUS_ERROR;
        }
    }
    return LR1110_HAL_STATUS_OK;
}


/*!
 * @brief                       Wrapper for SPI write communication
 *
 * @param[in] context           Radio abstraction
 * @param[in] command           Pointer to command bytes
 * @param[in] command_length
 *
 * @note                        NSS line is toggled manually.
 */
static void lr1110_spi_write(const void * context,
                             const uint8_t * command,
                             uint16_t command_length)
{
    struct spi_buf tx_buf = {
        .buf = (uint8_t*) command,
        .len = command_length
    };

    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1
    };

    spi_write(spi_dev, &spi_cfg, &tx);
}


/*!
 * @brief                       Wrapper for SPI read communication
 *
 * @param[in] context           Radio abstraction
 * @param[out] data             Pointer to data array, that comes from LR1110
 * @param[in] data_length       
 *
 * @note                        NSS line is toggled manually.
 */
static void lr1110_spi_read(const void * context,
                            //uint8_t *command, 
                            //uint8_t size_command,
                            uint8_t * data, 
                            uint16_t data_length)
{
    struct spi_buf rx_buf = {
        .buf = data,
        .len = data_length
    };

    const struct spi_buf_set rx = {
        .buffers = &rx_buf,
        .count = 1
    };

    spi_read(spi_dev, &spi_cfg, &rx);
}

/*!
 * @brief                       Wrapper for SPI read communication
 *
 * @param[in] context           Radio abstraction
 * @param[in] command           
 * @param[out] data             Pointer to data array, that comes from LR1110
 * @param[in] data_length       Length of command and data
 *
 * @note                        NSS line is toggled manually.
 */
static void lr1110_spi_write_read(const void * context, 
                                  const uint8_t * command, 
                                  uint8_t * data,
                                  const uint16_t data_length)
{
    const struct spi_buf tx_buf = {
        .buf = (uint8_t*) command,
        .len = data_length
    };
    struct spi_buf rx_buf = {
        .buf = data,
        .len = data_length
    };

    const struct spi_buf_set tx = {
      .buffers = &tx_buf,
      .count = 1
    };
    struct spi_buf_set rx = {
      .buffers = &rx_buf,
      .count = 1
    };

    spi_transceive(spi_dev, &spi_cfg, &tx, &rx);
}

/*** end of file ***/
