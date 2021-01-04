/** @file lr1110_configs.c
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

/* -------------------------------------------------------------------------
 * PRIVATE VARIABLES
 * ------------------------------------------------------------------------- */

/*!
 * @brief Port labels found in generated device tree source, this can be further
 *        added, however it should be also updated in create_port_pin funciton.  
 */
typedef enum 
{
    ZEPHYR_GPIO_0 = 0x00,    
    ZEPHYR_GPIO_1,
    ZEPHYR_GPIO_2,
    ZEPHYR_GPIO_3,
    ZEPHYR_GPIOA,
    ZEPHYR_GPIOB,
    ZEPHYR_GPIOC,
    ZEPHYR_GPIOD,
    ZEPHYR_GPIOE,
} port_label_t; 


/* -------------------------------------------------------------------------
 * PRIVATE PROTOTYPES
 * ------------------------------------------------------------------------- */
static void lr1110_set_nrf52832_config(void * context);
static void lr1110_set_nrf52840_config(void * context);
static void lr1110_set_nrf9160_config(void * context);
static port_pin_t create_port_pin(port_label_t port, gpio_pin_t pin);


/* -------------------------------------------------------------------------
 * PUBLIC IMPLEMENTATIONS
 * ------------------------------------------------------------------------- */

/*!
 * @brief               Sets device config 
 *
 * @param[in] device    Device name in string representation 
 *
 * @note                List of possible devices has to be mainted 
 *                      in README file.
 */
void lr1110_set_config(void * context, const char * device)
{
    if (strcmp(device, "NRF52832") == 0)
    {
        lr1110_set_nrf52832_config(context);
    }
    else if (strcmp(device, "NRF52840") == 0) 
    {
        lr1110_set_nrf52840_config(context);
    }
    else if (strcmp(device, "NRF9160") == 0) 
    {
        lr1110_set_nrf9160_config(context);
    }
    else
    {
        printk("ERROR: INCORRECT DEVICE CONFIG\n");
        while(1);
    }
}

/* -------------------------------------------------------------------------
 * PRIVATE IMPLEMENTATIONS
 * ------------------------------------------------------------------------- */


/*!
 * @brief               Creates a defult pin config for LR1110 evk
 *                      shield and nrf52832 combo 
 *
 * @param[in] context   Radio abstraction
 */
static void lr1110_set_nrf52832_config(void * context)
{
    ((lr1110_t*) context)->reset = create_port_pin(ZEPHYR_GPIO_0, 3);
    ((lr1110_t*) context)->nss   = create_port_pin(ZEPHYR_GPIO_0, 18);
    ((lr1110_t*) context)->event = create_port_pin(ZEPHYR_GPIO_0, 17);
    ((lr1110_t*) context)->busy  = create_port_pin(ZEPHYR_GPIO_0, 14);
    ((lr1110_t*) context)->lna   = create_port_pin(ZEPHYR_GPIO_0, 29);
    ((lr1110_t*) context)->spi_dev_label = "SPI_2";

    /* Default, EVK shield configuration is used */ 
    ((lr1110_t*) context)->rf_switch_cfg = NULL;
    ((lr1110_t*) context)->event_interrupt_cb = NULL;
    ((lr1110_t*) context)->event_trigger_type = GPIO_INT_LEVEL_HIGH;
}


/*!
 * @brief               Creates a defult pin config for LR1110 evk
 *                      shield and nrf52840 combo 
 *
 * @param[in] context   Radio abstraction
 */
static void lr1110_set_nrf52840_config(void * context)
{
    ((lr1110_t*) context)->reset = create_port_pin(ZEPHYR_GPIO_0, 3);
    ((lr1110_t*) context)->nss   = create_port_pin(ZEPHYR_GPIO_1, 8);
    ((lr1110_t*) context)->event = create_port_pin(ZEPHYR_GPIO_1, 6);
    ((lr1110_t*) context)->busy  = create_port_pin(ZEPHYR_GPIO_1, 4);
    ((lr1110_t*) context)->lna   = create_port_pin(ZEPHYR_GPIO_0, 29);
    ((lr1110_t*) context)->spi_dev_label = "SPI_3";

    /* Default, EVK shield configuration is used */ 
    ((lr1110_t*) context)->rf_switch_cfg = NULL;
    ((lr1110_t*) context)->event_interrupt_cb = NULL;
    ((lr1110_t*) context)->event_trigger_type = GPIO_INT_LEVEL_HIGH;
}


/*!
 * @brief               Creates a defult pin config for LR1110 evk
 *                      shield and NRF9160DK combo, shield is not directly
 *                      connected to DK, but with wires as some pins could not
 *                      be used. On board NRF52840 has to be properly 
 *                      configurated as well.
 *
 * @param[in] context   Radio abstraction
 */
static void lr1110_set_nrf9160_config(void * context)
{
    ((lr1110_t*) context)->reset = create_port_pin(ZEPHYR_GPIO_0, 14);
    ((lr1110_t*) context)->nss   = create_port_pin(ZEPHYR_GPIO_0, 7);
    ((lr1110_t*) context)->event = create_port_pin(ZEPHYR_GPIO_0, 5);
    ((lr1110_t*) context)->busy  = create_port_pin(ZEPHYR_GPIO_0, 3);
    ((lr1110_t*) context)->lna   = create_port_pin(ZEPHYR_GPIO_0, 17);
    ((lr1110_t*) context)->spi_dev_label = "SPI_3";

    /* Default, EVK shield configuration is used */ 
    ((lr1110_t*) context)->rf_switch_cfg = NULL;
    ((lr1110_t*) context)->event_interrupt_cb = NULL;
    ((lr1110_t*) context)->event_trigger_type = GPIO_INT_LEVEL_HIGH;
}


/*!
 * @brief                   Helper function that creates port_pin struct
 *
 * @param[in] Port          Ports are written exactly the same as they appear in 
 *                          device tree, prepended with ZEPHYR_.
 *                          This is done to avoid clashes with some other 
 *                          libraries.
 * @param[in] Pin           
 * @return port_pin struct
 */
static port_pin_t create_port_pin(port_label_t port, gpio_pin_t pin)
{
    const char * port_label;

    switch (port)
    {
        case ZEPHYR_GPIO_0: port_label = "GPIO_0"; break; 
        case ZEPHYR_GPIO_1: port_label = "GPIO_1"; break; 
        case ZEPHYR_GPIO_2: port_label = "GPIO_2"; break; 
        case ZEPHYR_GPIO_3: port_label = "GPIO_3"; break; 
        case ZEPHYR_GPIOA:  port_label = "GPIOA";  break; 
        case ZEPHYR_GPIOB:  port_label = "GPIOB";  break; 
        case ZEPHYR_GPIOC:  port_label = "GPIOC";  break; 
        case ZEPHYR_GPIOD:  port_label = "GPIOD";  break; 
        case ZEPHYR_GPIOE:  port_label = "GPIOE";  break; 

        default:
            printk("This GPIO port is not supported!\n");
            while(1);
        break; 
    }
    struct device * port_device = device_get_binding(port_label);

    port_pin_t created_port_pin = {
        .port = port_device, 
        .pin = pin
    };
    return created_port_pin;
}
/*** end of file ***/
