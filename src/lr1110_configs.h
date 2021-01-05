/** @file lr1110_configs.c
 * 
 * @brief       Main lr1110.h file that calls all other modules. 
 *              Only this file should be included into application.
 *      
 *
 * @par       
 * COPYRIGHT NOTICE: (c) 2021 Irnas.  All rights reserved.
 */ 
 
#ifndef LR1110_CONFIGS_H
#define LR1110_CONFIGS_H

#ifdef __cplusplus
extern "C" {
#endif

void lr1110_set_config(void * context, const char * device);

#ifdef __cplusplus
}
#endif

#endif  // LR1110_CONFIGS_H

/*** end of file ***/
