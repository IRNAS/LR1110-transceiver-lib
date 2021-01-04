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
{}

void lr1110_set_device_config(const void * context, const char * device)
{
    lr1110_set_config(context, device);
}

/* -------------------------------------------------------------------------
 * PRIVATE IMPLEMENTATIONS
 * ------------------------------------------------------------------------- */

/*** end of file ***/
