 #include "systick.h"
// For store tick counts in us
static __IO uint32_t usTicks;

// SysTick_Handler function will be called every 1 us
void SysTick_Handler()
{
	if (usTicks != 0)
	{
		usTicks--;
	}
}

void systick_config()
{
	// Update SystemCoreClock value
	//SystemCoreClockUpdate();
	// Configure the SysTick timer to overflow every 1 us
	SysTick_Config(SystemCoreClock / 1000000);
	NVIC_SetPriority(SysTick_IRQn,0x00U);
}

void delay_1us(uint32_t us)
{
	// Reload us value
	usTicks = us;
	// Wait until usTick reach zero
	while (usTicks);
}

void delay_1ms(uint32_t ms)
{
	// Wait until ms reach zero
	while (ms--)
	{
		// Delay 1ms
		delay_1us(1000);
	}
}