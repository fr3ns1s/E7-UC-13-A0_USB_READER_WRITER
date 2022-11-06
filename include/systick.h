#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>
#include "gd32f10x.h"

void systick_config(void);
void delay_1ms(uint32_t count);
void delay_1us(uint32_t count);

#endif /* SYSTICK_H */
