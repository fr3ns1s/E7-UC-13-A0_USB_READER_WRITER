#include "fm1702.h"

#define RST_HIGH                gpio_bit_set  (FM1702_GPIO_RST,FM1702_RST)
#define RST_LOW                	gpio_bit_reset(FM1702_GPIO_RST,FM1702_RST)
#define CS_HIGH           			gpio_bit_set  (FM1702_GPIO,FM1702_NSS)
#define CS_LOW            			gpio_bit_reset(FM1702_GPIO,FM1702_NSS)	

uint8_t drv_spiReadWriteByte(uint8_t byte)
{
	
	while (spi_i2s_flag_get(FM1702_SPI, SPI_FLAG_TBE) == RESET){;} 
	spi_i2s_data_transmit(FM1702_SPI,byte);
	while (spi_i2s_flag_get(FM1702_SPI, SPI_FLAG_RBNE) == RESET){;} 
	while (spi_i2s_flag_get(FM1702_SPI, SPI_FLAG_TRANS) == SET){;}
	return (spi_i2s_data_receive(FM1702_SPI)); 
}


void drv_spiInit(void)
{
	spi_parameter_struct spi_init_struct;

	rcu_periph_clock_enable(FM1702_RCU); 
	rcu_periph_clock_enable(FM1702_RST_RCU);
	rcu_periph_clock_enable(FM1702_SPI_RCU); 
  rcu_periph_clock_enable(RCU_AF);

	gpio_init(FM1702_GPIO, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, FM1702_SCK|FM1702_MOSI|FM1702_MISO);
	gpio_init(FM1702_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, FM1702_NSS);
	gpio_init(FM1702_GPIO_RST, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, FM1702_RST);

	spi_i2s_deinit(FM1702_SPI);
	
	spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX; 
	spi_init_struct.device_mode          = SPI_MASTER;              
	spi_init_struct.nss                  = SPI_NSS_SOFT;            
	spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;    
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
	spi_init_struct.prescale             = SPI_PSC_256;            
	spi_init_struct.endian               = SPI_ENDIAN_MSB;          
	
	CS_HIGH;
	
	spi_init(FM1702_SPI, &spi_init_struct);
	//spi_crc_polynomial_set(FM1702_SPI,7); 
	spi_enable(FM1702_SPI);              
	drv_spiReadWriteByte(0xff);
}

void drv_fm1702ItfGpioInit(void)
{
	drv_spiInit();
}

void drv_fm1702ItfClearReset(void)
{
	RST_LOW;
}

void drv_fm1702ItfSetReset(void)
{
	RST_HIGH;
}

void drv_fm1702ItfClearCs(void)
{
	CS_LOW;
}

void drv_fm1702ItfSetCs(void)
{
	CS_HIGH;
}

//=======================================================
//	??:	drv_fm1702ItfReadByte
//	??:	SPI????
//	??:
//		addr:	????FM1702SL???????[0x01~0x3f]
//	??:
//	??:??????,???????????
//========================================================
uint8_t drv_fm1702ItfReadByte(uint8_t addr)
{
    uint8_t rdata;
    
    drv_fm1702ItfClearCs();
	
    addr <<= 1;
    addr |= 0x80;
	
	drv_spiReadWriteByte(addr);
	rdata = drv_spiReadWriteByte(0x00);
	
    drv_fm1702ItfSetCs();
    
    return (rdata);
}

void drv_fm1702ItfRead(uint8_t addr, uint8_t* dat, uint8_t len)
{
	if(len == 0)
	{
		return;
	}
	
    drv_fm1702ItfClearCs();
	
    addr <<= 1;
    addr |= 0x80;
	
	drv_spiReadWriteByte(addr);
	
	while(len--)
	{
		*dat++ = drv_spiReadWriteByte(addr);
	}
	
	drv_fm1702ItfSetCs();
}


//=======================================================
//	??:drv_fm1702ItfWriteOneByte
//	??:SPI????
//	??:	addr:	???FM1702SL???????[0x01~0x3f]
//          wData :  	??????
//	??:
//	??:??????,???????????
//========================================================
void drv_fm1702ItfWriteByte(uint8_t addr, uint8_t byte)
{
    drv_fm1702ItfClearCs();
	
    addr <<= 1;
    addr &= 0x7E;

	drv_spiReadWriteByte(addr);
	
	drv_spiReadWriteByte(byte);
	
    drv_fm1702ItfSetCs();
}

void drv_fm1702ItfWrite(uint8_t addr, uint8_t* dat, uint8_t len)
{
	if(len == 0)
	{
		return ;
	}
	
    drv_fm1702ItfClearCs();
	
    addr <<= 1;
    addr &= 0x7E;

	drv_spiReadWriteByte(addr);
	
	while(len--)
	{
		drv_spiReadWriteByte(*dat++);
	}
	
    drv_fm1702ItfSetCs();
}