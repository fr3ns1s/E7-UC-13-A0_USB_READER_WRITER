#include "board_conf.h"
#include "keyboard.h"
#include "lcd.h"
#include "main.h"

#define SDA_HIGH gpio_bit_set(KEYBOARD_PORT,KEYBOARD_SDA_PIN)
#define SDA_LOW gpio_bit_reset(KEYBOARD_PORT,KEYBOARD_SDA_PIN)
#define SCL_HIGH gpio_bit_set(KEYBOARD_PORT,KEYBOARD_SCL_PIN)
#define SCL_LOW gpio_bit_reset(KEYBOARD_PORT,KEYBOARD_SCL_PIN)
#define ACK_STATE gpio_input_bit_get(KEYBOARD_PORT, KEYBOARD_SDA_PIN)


uint8_t I2C_Read_Byte(uint8_t reg_addr);
void I2C_Write_Byte(uint8_t reg_addr,uint8_t data);
uint8_t Get_edition(void);


void GPIO_init(void) 
{
	
	rcu_periph_clock_enable(KEYBOARD_RCU);
	rcu_periph_clock_enable(RCU_AF);
	gpio_init(KEYBOARD_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, KEYBOARD_SDA_PIN);
	gpio_init(KEYBOARD_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, KEYBOARD_SCL_PIN);
  gpio_init(KEYBOARD_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEYBOARD_INT_PIN);
	
}
	
	
uint8_t Keyboard_config(void)
	{ 

		I2C_Write_Byte(0x6E,0x80);
		if(Get_edition())
		{ 
				I2C_Write_Byte(0x68,0x22); 
				return 1;
		}

		return 0;
	}


void EXTIX_init(void)
	{
 
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(EXTI5_9_IRQn, 2U, 2U);
	  gpio_exti_source_select(KEYBOARD_INT_PORT_SOURCE, KEYBOARD_INT_PIN_SOURCE);
    exti_init(EXTI_5, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_5);
	  exti_interrupt_enable(EXTI_5);

	}
	
	 void EXTI5_9_IRQHandler(void)
	 {
	
			if (RESET != exti_interrupt_flag_get(EXTI_5))
				{
					Keyboard_scan();
					exti_interrupt_flag_clear(EXTI_5);
				}
	 }


void Keyboard_init(void)
	{
		GPIO_init();
		EXTIX_init();
		Keyboard_config();
	}

	
void Keyboard_scan(void)
	{
		uint8_t crd = -1;
		uint8_t readed = I2C_Read_Byte(0x6F);
		keyboard_interrupt_delegate(readed);
	}


void SDA_IN_Config(void)
	{
		gpio_init(KEYBOARD_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEYBOARD_SDA_PIN);
	}

void SDA_OUT_Config(void)
	{
		gpio_init(KEYBOARD_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, KEYBOARD_SDA_PIN);
	}

uint8_t Get_edition(void)
	{ 
		uint8_t edition_value;
		I2C_Write_Byte(0x60,0x40); 
		edition_value = I2C_Read_Byte(0x61); 

		if(edition_value == 0x20) {
			return 1;
		}
		 
		return 0;
	}

void I2C_Stop(void)
	{
		delay_1us(50); 
		SCL_HIGH;
		delay_1us(50);
		SDA_HIGH;
		delay_1us(50);
	}

void I2C_Start(void)
	{
		SDA_HIGH;
		SCL_HIGH;
		delay_1us(50);
		SDA_LOW;
		delay_1us(50);
		SCL_LOW; 
	}

void data_trans(uint8_t outdata)
	{
		uint8_t i; 
		for(i=0;i<8;i++)
		{ 
			delay_1us(20); 
			if(outdata & 0x80)
			SDA_HIGH;
			else
			SDA_LOW; 
			delay_1us(30);
			SCL_HIGH; 
			delay_1us(50);
			SCL_LOW; 
			outdata=outdata<<1;
		}
		delay_1us(50);
		SDA_IN_Config(); 
		delay_1us(10);
		SCL_HIGH;
	}

uint8_t data_receive(void)
	{
		uint8_t indata=0x00; 
		uint8_t i;
		for(i=0;i<8;i++)
		{
			indata = indata<<1;
			delay_1us(50);
			SCL_HIGH; 
			if(ACK_STATE)
			indata=indata+1; 
			delay_1us(50);
			SCL_LOW;
		}
		delay_1us(50);
		SCL_HIGH; 
		return indata; 
	}

void I2C_Write_Byte(uint8_t reg_addr,uint8_t data)
	{
		I2C_Start();
		data_trans(reg_addr);
		while(ACK_STATE);
		delay_1us(50);
		SDA_OUT_Config(); 
		SCL_LOW;
		data_trans(data);
		while(ACK_STATE);
		delay_1us(50);
		SCL_LOW;
		SDA_OUT_Config();
		I2C_Stop();
	}

uint8_t I2C_Read_Byte(uint8_t reg_addr)
	{ 
		uint8_t temp_data;
		I2C_Start();
		data_trans(reg_addr);
		while(ACK_STATE);
		delay_1us(50);
		SCL_LOW;
		temp_data=data_receive();
		delay_1us(50);
		SDA_OUT_Config();
		I2C_Stop();
		return (temp_data);
	}

