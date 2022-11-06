#ifndef __lcd_h
#define __lcd_h

#include "board_conf.h"

void LCD_init(void);
void LCD_Write(int row, int col, char* string);
void LCD_Write_Rows(char *stringRow0, char *stringRow1);
#endif