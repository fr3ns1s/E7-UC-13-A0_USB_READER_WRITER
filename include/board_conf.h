#ifndef __board_conf_h
#define __board_conf_h

#include "gd32f10x.h"

//PINOUT PROGRAMMING PORT
//VDD
//PB4 - NJRST
//PA15 - JTDI
//PA13 - SWDIO - JTMS
//PA14 SWCLK  - JTCK
//NRST
//PB3 - JTDO
//GND

#define USB_PULLUP                        GPIOB
#define USB_PULLUP_PIN                    GPIO_PIN_9
#define RCU_USB_PULLUP          					RCU_GPIOB

#define BUZZER_PIN                       	GPIO_PIN_3
#define BUZZER_GPIO_PORT                 	GPIOD
#define BUZZER_GPIO_CLK                  	RCU_GPIOD

#define LED_GREEN_PIN                     GPIO_PIN_7
#define LED_GREEN_GPIO_PORT               GPIOD
#define LED_GREEN_GPIO_CLK                RCU_GPIOD

#define LED_BLUE_PIN                      GPIO_PIN_0
#define LED_BLUE_GPIO_PORT                GPIOE
#define LED_BLUE_GPIO_CLK                 RCU_GPIOE

#define RCU_LCD														RCU_GPIOC
#define LCD_PORT 													GPIOC
#define LCD_SCK_PIN 											GPIO_PIN_12
#define LCD_CS_PIN 												GPIO_PIN_10
#define LCD_SID_PIN 											GPIO_PIN_11
#define RCU_LCD_LIGHT 										RCU_GPIOD
#define LCD_LIGHT_PORT 										GPIOD
#define LCD_LIGHT_PIN 										GPIO_PIN_0

#define KEYBOARD_PORT 										GPIOB
#define KEYBOARD_RCU 											RCU_GPIOB
#define KEYBOARD_SDA_PIN 									GPIO_PIN_7
#define KEYBOARD_SCL_PIN 									GPIO_PIN_6
#define KEYBOARD_INT_PIN 									GPIO_PIN_5
#define KEYBOARD_INT_PORT_SOURCE					GPIO_PORT_SOURCE_GPIOB
#define KEYBOARD_INT_PIN_SOURCE						GPIO_PIN_SOURCE_5

#define AT24_I2CX                   		 	I2C1
#define AT24_RCU_GPIO_I2C            			RCU_GPIOB
#define AT24_RCU_I2C                 			RCU_I2C1
#define AT24_PORT            							GPIOB
#define AT24_SCL_PIN             					GPIO_PIN_10
#define AT24_SDA_PIN             					GPIO_PIN_11

#define FM1702_SPI 												SPI0
#define FM1702_SPI_RCU 										RCU_SPI0
#define FM1702_RST_RCU 										RCU_GPIOC
#define FM1702_RCU 												RCU_GPIOA
#define FM1702_GPIO_RST 									GPIOC
#define FM1702_GPIO 											GPIOA
#define FM1702_MOSI 											GPIO_PIN_7
#define FM1702_MISO												GPIO_PIN_6
#define FM1702_SCK												GPIO_PIN_5
#define FM1702_NSS												GPIO_PIN_4
#define FM1702_RST												GPIO_PIN_4


//NOT_TESTED
#define SMART_CARD_RCU 										RCU_GPIOA
#define SMART_CARD_GPIO 									GPIOA
#define SMART_CARD_RST 										GPIO_PIN_9
#define SMART_CARD_CLK										GPIO_PIN_11
#define SMART_CARD_IO											GPIO_PIN_8

#define SIM_CARD_CLK_RCU 									RCU_GPIOA
#define SIM_CARD_CLK_GPIO									GPIOA
#define SIM_CARD_CLK											GPIO_PIN_11
#define SIM_CARD_IO_RCU 									RCU_GPIOB
#define SIM_CARD_IO_GPIO									GPIOB
#define SIM_CARD_1_IO											GPIO_PIN_10
#define SIM_CARD_2_IO											GPIO_PIN_9
#define SIM_CARD_3_IO											GPIO_PIN_18
#define SIM_CARD_RST_RCU 									RCU_GPIOB
#define SIM_CARD_RST_GPIO									GPIOB
#define SIM_CARD_RST											GPIO_PIN_4

#endif