#include "lcd.h"

uint8_t GLCD_Buf[1024];

#define SID_HIGH 		gpio_bit_set(LCD_PORT, LCD_SID_PIN)
#define SID_LOW 		gpio_bit_reset(LCD_PORT, LCD_SID_PIN)

#define SCK_HIGH		gpio_bit_set(LCD_PORT, LCD_SCK_PIN)
#define SCK_LOW 		gpio_bit_reset(LCD_PORT, LCD_SCK_PIN)

#define CS_HIGH			gpio_bit_set(LCD_PORT, LCD_CS_PIN)
#define CS_LOW 			gpio_bit_reset(LCD_PORT, LCD_CS_PIN)


static void SendByteSPI(uint8_t byte)
{
	
	
	for(int i=0;i<8;i++)
	{
		if((byte<<i)&0x80)
			{
				SID_HIGH;  // SID=1
			}

		else {SID_LOW;}  // SID=0

		SCK_LOW;  // SCLK =0  OR SCK
		SCK_HIGH;  // SCLK=1
}
		
}

static void ST7920_SendCmd (uint8_t cmd)
{

	//CS_HIGH;  // PUll the CS high

	CS_HIGH;
	
	SendByteSPI(0xf8+(0<<1));  // send the SYNC + RS(0)
	SendByteSPI(cmd&0xf0);  // send the higher nibble first
	SendByteSPI((cmd<<4)&0xf0);  // send the lower nibble
	
	CS_LOW;
	SCK_LOW;
	
}

static void ST7920_SendData (uint8_t data)
{

	
	CS_HIGH;

	SendByteSPI(0xf8+(1<<1));  // send the SYNC + RS(1)
	SendByteSPI(data&0xf0);  // send the higher nibble first
	SendByteSPI((data<<4)&0xf0);  // send the lower nibble

	CS_LOW;
	SCK_LOW;
	
	delay_1us(25);
	
}

void clearRow(int row) {
	
	ST7920_SendCmd(0x30); 
	delay_1ms(25); 
	LCD_Write(row,0,"                  ");

}

void clear_display(void) {
	
	clearRow(0);
	delay_1ms(10); 
	clearRow(1);
	delay_1ms(10);
	
}


void LCD_Write(int row, int col, char* string)
{
	
	ST7920_SendCmd(0x30); 
	delay_1ms(25);
	
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0x90;
            break;
        case 2:
            col |= 0xA0;
            break;
        case 3:
            col |= 0xB0;
            break;
        default:
            col |= 0x80;
            break;
    }

    ST7920_SendCmd(col);
		delay_1ms(5);
		
    while (*string)
    	{
    		ST7920_SendData(*string++);
    	}
}


void LCD_init (void)
{
	
	rcu_periph_clock_enable(RCU_LCD);
	rcu_periph_clock_enable(RCU_LCD_LIGHT);

	gpio_init(LCD_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_SCK_PIN|LCD_SID_PIN|LCD_CS_PIN);
	gpio_init(LCD_LIGHT_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
  gpio_bit_set(LCD_LIGHT_PORT, LCD_LIGHT_PIN);
	
	CS_LOW;
	SCK_LOW;
	SID_LOW;
	
  delay_1ms(5);
	
	ST7920_SendCmd(0x06);  // 8bit mode
	delay_1us(200);  //  >100us delay

	ST7920_SendCmd(0x0c);  // 8bit mode
	delay_1us(80);  // >37us delay

	ST7920_SendCmd(0x01);  // D=0, C=0, B=0
	delay_1us(2000);  // >100us delay
	
	ST7920_SendCmd(0x30);  // clear screen
	delay_1ms(30);  // >10 ms delay
	
}

void LCD_Write_Rows(char *stringRow0, char *stringRow1) {
	
	clear_display();
	LCD_Write(0,0,stringRow0);
	delay_1ms(5);
	LCD_Write(1,0,stringRow1);
	
}
