#include <stdio.h>
#include "main.h"
#include "board_conf.h"
#include "systick.h"
#include "cdc_acm_core.h"
#include "lcd.h"
#include "keyboard.h"
#include "at24cxx.h"
#include "fm1702.h"


#define DEBUG 1

#ifdef DEBUG 
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f)
{
  ITM_SendChar(ch);
  return(ch);
}
#endif


void buzzer_init(void) {
    rcu_periph_clock_enable(BUZZER_GPIO_CLK);
    gpio_init(BUZZER_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, BUZZER_PIN);
	  GPIO_BOP(BUZZER_GPIO_PORT) = BUZZER_PIN;
}

void buzzer_ON_for(uint32_t millis) {
	
	 GPIO_BC(BUZZER_GPIO_PORT) = BUZZER_PIN;
   delay_1ms(millis);
   GPIO_BOP(BUZZER_GPIO_PORT) = BUZZER_PIN;
    
}

void leds_init(void) {
    rcu_periph_clock_enable(LED_GREEN_GPIO_CLK);
    gpio_init(LED_GREEN_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED_GREEN_PIN);
		GPIO_BC(LED_GREEN_GPIO_PORT) = LED_GREEN_PIN;
   
		rcu_periph_clock_enable(LED_BLUE_GPIO_CLK);
    gpio_init(LED_BLUE_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED_BLUE_PIN);
		GPIO_BC(LED_BLUE_GPIO_PORT) = LED_BLUE_PIN;
}

void led_BLUE_ON_for(uint32_t millis) {
	
	GPIO_BOP(LED_BLUE_GPIO_PORT) = LED_BLUE_PIN;
	delay_1ms(millis);
	GPIO_BC(LED_BLUE_GPIO_PORT) = LED_BLUE_PIN;
	
}

void led_GREEN_ON_for(uint32_t millis) {
	
	GPIO_BOP(LED_GREEN_GPIO_PORT) = LED_GREEN_PIN;
	delay_1ms(millis);
	GPIO_BC(LED_GREEN_GPIO_PORT) = LED_GREEN_PIN;
}


#ifdef USB
usb_dev usbd_cdc;

void init_usb_cdc(void) {
		
		rcu_config();
		gpio_config();
		usbd_init(&usbd_cdc, &cdc_desc, &cdc_class);
    nvic_config();
    usbd_connect(&usbd_cdc);

		while (USBD_CONFIGURED != usbd_cdc.cur_status) {
        /* wait for standard USB enumeration is finished */
    }
		
		buzzer_ON_for(100);
}
#endif

void log_to_vcom(char *data) {
	#ifdef USB
		cdc_acm_data_send(&usbd_cdc,data,strlen(data)); 
	#endif
}



void init_rounites(void)
	{
		buzzer_init();
	  leds_init();
		eeprom_init();
		Keyboard_init();
		while(!drv_fm1702Init());
		led_BLUE_ON_for(1000);
		led_GREEN_ON_for(1000);
		LCD_init();
		
	}
	
int main(void)
	{
    systick_config();
		init_rounites();
		
		#ifdef USB
			init_usb_cdc
		#endif
		
		delay_1ms(1000);
		buzzer_ON_for(100);
		
		LCD_Write_Rows("   HELLO WORLD!   ","  2022 - FR3NSIS  ");
		
		
		while(1) {
			
		}
	}


void change_menu(int value) {
 
	printf("Menu selected %i\n",value);
}

void value_selected(int value) {
 
	printf("Value selected %i\n",value);
}

void OK_button_pressed(void) {
	
	printf("OK\n");
}

void CLR_button_pressed(void) {
	
	printf("CLR\n");
}

//KEYBOARD SECTION
void keyboard_interrupt_delegate(uint8_t value) {
	
	switch(value) 
		{
			case(0x47):
				// "F1";
			  change_menu(0);
				return;
			case(0x46):
				// "F2";
				change_menu(1);
				return;
			case(0x45):
				// "F3";
				change_menu(2);
				return;
			case(0x4F):
				// "1";
				value = 1;
				break;
			case(0x4E):
				// "2";
				value = 2;
				break;
			case(0x4D):
				// "3";
				value = 3;
				break;
			case(0x57):
				// "4";
				value = 4;
				break;
			case(0x56):
				// "5";
				value = 5;
				break;
			case(0x55):
				// "6";
				value = 6;
				break;
			case(0x5F):
				// "7";
				value = 7;
				break;
			case(0x5E):
				// "8";
				value = 8;
				break;
			case(0x5D):
				// "9";
				value = 9;
				break;
			case(0x5C):
				// "CLR";
				CLR_button_pressed();
				break;
			case(0x54):
				// "0";
				value = 0;
				break;
			case(0x4C):
				// "OK";
			  OK_button_pressed();
				return;
		}
		
		value_selected(value);
}
