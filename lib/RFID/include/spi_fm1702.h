#ifndef __spi_fm1702_h
#define __spi_fm1702_h

#include "board_conf.h"


void drv_fm1702ItfGpioInit(void);
void drv_fm1702ItfClearReset(void);
void drv_fm1702ItfSetReset(void);
void drv_fm1702ItfClearCs(void);
void drv_fm1702ItfSetCs(void);
uint8_t drv_fm1702ItfReadByte(uint8_t addr);
void drv_fm1702ItfWriteByte(uint8_t addr, uint8_t byte);
void drv_fm1702ItfWrite(uint8_t addr, uint8_t* dat, uint8_t len);
void drv_fm1702ItfRead(uint8_t addr, uint8_t* dat, uint8_t len);
void drv_spiInit(void);
#endif

