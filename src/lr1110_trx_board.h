/** @file lr1110_trx_board.h
 * 
 * @brief This module deals with GPIO and SPI initializations in connection
 *        with lr1110 chip. It implements functions declared in lr1110_hal.h, 
 *        as well in lr1110-modem-board.h
 *
 * @par       
 * COPYRIGHT NOTICE: (c) 2020 Irnas.  All rights reserved.
 */ 
 
#ifndef LR1110_TRX_BOARD_H
#define LR1110_TRX_BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

void lr1110_gpio_init(const void * context);
void lr1110_spi_init(const void * context);

#ifdef __cplusplus
}
#endif

#endif  // LR1110_TRX_BOARD_H

/*** end of file ***/
