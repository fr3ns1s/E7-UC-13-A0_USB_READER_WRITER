/*!
    \file    at24cxx.c
    \brief   the read and write function file

    \version 2020-09-30, V1.0.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "at24cxx.h"
#include <stdio.h>
#include "main.h"

#define EEPROM_BLOCK0_ADDRESS    0xA0
#define BUFFER_SIZE              8
#define I2C_SPEED               100000
#define AT24_I2CX_SLAVE_ADDRESS7     0xA0
#define I2C_PAGE_SIZE 8

char temp_str[20];
uint16_t eeprom_address;

void i2c_eeprom_init(void);
uint8_t eeprom_byte_write_timeout(uint8_t *p_buffer, uint8_t write_address);
uint8_t eeprom_page_write_timeout(uint8_t *p_buffer, uint8_t write_address, uint8_t number_of_byte);
void eeprom_buffer_write_timeout(uint8_t *p_buffer, uint8_t write_address, uint16_t number_of_byte);
uint8_t eeprom_buffer_read_timeout(uint8_t *p_buffer, uint8_t read_address, uint16_t number_of_byte);
uint8_t eeprom_wait_standby_state_timeout(void);

void at24_gpio_config(void)
{
    rcu_periph_clock_enable(AT24_RCU_GPIO_I2C);
    gpio_init(AT24_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, AT24_SCL_PIN);
    gpio_init(AT24_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, AT24_SDA_PIN);
}

/*!
    \brief      configure the AT24_I2CX interfaces
    \param[in]  none
    \param[out] none
    \retval     none
*/
void at24_i2c_config(void)
{
    /* enable I2C clock */
    rcu_periph_clock_enable(AT24_RCU_I2C);
    /* configure I2C clock */
    i2c_clock_config(AT24_I2CX, I2C_SPEED, I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(AT24_I2CX, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, AT24_I2CX_SLAVE_ADDRESS7);
    /* enable AT24_I2CX */
    i2c_enable(AT24_I2CX);
    /* enable acknowledge */
    i2c_ack_config(AT24_I2CX, I2C_ACK_ENABLE);
}

/*!
    \brief      reset i2c bus
    \param[in]  none
    \param[out] none
    \retval     none
*/

void at24_i2c_bus_reset()
{
    i2c_deinit(AT24_I2CX);
    /* configure SDA/SCL for GPIO */
    GPIO_BC(AT24_PORT) |= AT24_SCL_PIN;
    GPIO_BC(AT24_PORT) |= AT24_SDA_PIN;
    gpio_init(AT24_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, AT24_SCL_PIN);
    gpio_init(AT24_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, AT24_SDA_PIN);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    GPIO_BOP(AT24_PORT) |= AT24_SCL_PIN;
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    GPIO_BOP(AT24_PORT) |= AT24_SDA_PIN;
    /* connect AT24_SCL_PIN to I2C_SCL */
    /* connect AT24_SDA_PIN to I2C_SDA */
    gpio_init(AT24_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, AT24_SCL_PIN);
    gpio_init(AT24_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, AT24_SDA_PIN);
    /* configure the AT24_I2CX interface */
    at24_i2c_config();
}


void eeprom_init(void) 
{

	at24_gpio_config();
	at24_i2c_config();
	i2c_eeprom_init();
 
}


/*!
    \brief      initialize peripherals used by the I2C EEPROM driver
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_eeprom_init(void)
{
    eeprom_address = EEPROM_BLOCK0_ADDRESS;
}

/*!
    \brief      write one byte to the EEPROM and use timeout function
    \param[in]  p_buffer: pointer to the buffer containing the data to be written to the EEPROM
    \param[in]  write_address: EEPROM's internal address to write to
    \param[out] none
    \retval     none
*/
uint8_t eeprom_byte_write_timeout(uint8_t *p_buffer, uint8_t write_address)
{
    uint8_t   state = I2C_START;
    uint16_t  timeout = 0;
    uint8_t   i2c_timeout_flag = 0;

    /* enable acknowledge */
    i2c_ack_config(AT24_I2CX, I2C_ACK_ENABLE);
    while(!(i2c_timeout_flag)) {
        switch(state) {
        case I2C_START:
            /* i2c master sends start signal only when the bus is idle */
            while(i2c_flag_get(AT24_I2CX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_start_on_bus(AT24_I2CX);
                timeout = 0;
                state = I2C_SEND_ADDRESS;
            } else {
                timeout = 0;
                state   = I2C_START;
                log_to_vcom("i2c bus is busy in WRITE BYTE!\n");
            }
            break;
        case I2C_SEND_ADDRESS:
            /* i2c master sends START signal successfully */
            while((! i2c_flag_get(AT24_I2CX, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_master_addressing(AT24_I2CX, eeprom_address, I2C_TRANSMITTER);
                timeout = 0;
                state = I2C_CLEAR_ADDRESS_FLAG;
            } else {
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c master sends start signal timeout in WRITE BYTE!\n");
            }
            break;
        case I2C_CLEAR_ADDRESS_FLAG:
            /* address flag set means i2c slave sends ACK */
            while((! i2c_flag_get(AT24_I2CX, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_flag_clear(AT24_I2CX, I2C_FLAG_ADDSEND);
                timeout = 0;
                state = I2C_TRANSMIT_DATA;
            } else {
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c master clears address flag timeout in WRITE BYTE!\n");
            }
            break;
        case I2C_TRANSMIT_DATA:
            /* wait until the transmit data buffer is empty */
            while((! i2c_flag_get(AT24_I2CX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* send the EEPROM's internal address to write to : only one byte address */
                i2c_data_transmit(AT24_I2CX, write_address);
                timeout = 0;
            } else {
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c master sends data timeout in WRITE BYTE!\n");
            }

            /* wait until BTC bit is set */
            while((!i2c_flag_get(AT24_I2CX, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* send the EEPROM's internal address to write to : only one byte address */
                i2c_data_transmit(AT24_I2CX, *p_buffer);
                timeout = 0;
            } else {
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c master sends data timeout in WRITE BYTE!\n");
            }
            /* wait until BTC bit is set */
            while((!i2c_flag_get(AT24_I2CX, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                state = I2C_STOP;
                timeout = 0;
            } else {
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c master sends data timeout in WRITE BYTE!\n");
            }
            break;
        case I2C_STOP:
            /* send a stop condition to I2C bus */
            i2c_stop_on_bus(AT24_I2CX);
            /* i2c master sends STOP signal successfully */
            while((I2C_CTL0(AT24_I2CX) & I2C_CTL0_STOP) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_END;
                i2c_timeout_flag = I2C_OK;
            } else {
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c master sends stop signal timeout in WRITE BYTE!\n");
            }
            break;
        default:
            state = I2C_START;
            i2c_timeout_flag = I2C_OK;
            timeout = 0;
            log_to_vcom("i2c master sends start signal in WRITE BYTE.\n");
            break;
        }
    }
    return I2C_END;
}
/*!
    \brief      write more than one byte to the EEPROM with a single write cycle
    \param[in]  p_buffer: pointer to the buffer containing the data to be written to the EEPROM
    \param[in]  write_address: EEPROM's internal address to write to
    \param[in]  number_of_byte: number of bytes to write to the EEPROM
    \param[out] none
    \retval     none
*/
uint8_t eeprom_page_write_timeout(uint8_t *p_buffer, uint8_t write_address, uint8_t number_of_byte)
{
    uint8_t   state = I2C_START;
    uint16_t  timeout = 0;
    uint8_t   i2c_timeout_flag = 0;

    /* enable acknowledge */
    i2c_ack_config(AT24_I2CX, I2C_ACK_ENABLE);
    while(!(i2c_timeout_flag)) {
        switch(state) {
        case I2C_START:
            /* i2c master sends start signal only when the bus is idle */
            while(i2c_flag_get(AT24_I2CX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_start_on_bus(AT24_I2CX);
                timeout = 0;
                state = I2C_SEND_ADDRESS;
            } else {
                at24_i2c_bus_reset();
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c bus is busy in WRITE!\n");
            }
            break;
        case I2C_SEND_ADDRESS:
            /* i2c master sends START signal successfully */
            while((! i2c_flag_get(AT24_I2CX, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_master_addressing(AT24_I2CX, eeprom_address, I2C_TRANSMITTER);
                timeout = 0;
                state = I2C_CLEAR_ADDRESS_FLAG;
            } else {
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c master sends start signal timeout in WRITE!\n");
            }
            break;
        case I2C_CLEAR_ADDRESS_FLAG:
            /* address flag set means i2c slave sends ACK */
            while((! i2c_flag_get(AT24_I2CX, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_flag_clear(AT24_I2CX, I2C_FLAG_ADDSEND);
                timeout = 0;
                state = I2C_TRANSMIT_DATA;
            } else {
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c master clears address flag timeout in WRITE!\n");
            }
            break;
        case I2C_TRANSMIT_DATA:
            /* wait until the transmit data buffer is empty */
            while((! i2c_flag_get(AT24_I2CX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* send the EEPROM's internal address to write to : only one byte address */
                i2c_data_transmit(AT24_I2CX, write_address);
                timeout = 0;
            } else {
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c master sends EEPROM's internal address timeout in WRITE!\n");
            }
            /* wait until BTC bit is set */
            while((!i2c_flag_get(AT24_I2CX, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                timeout = 0;
            } else {
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c master sends data timeout in WRITE!\n");
            }
            while(number_of_byte--) {
                i2c_data_transmit(AT24_I2CX, *p_buffer);
                /* point to the next byte to be written */
                p_buffer++;
                /* wait until BTC bit is set */
                while((!i2c_flag_get(AT24_I2CX, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    timeout = 0;
                } else {
                    timeout = 0;
                    state = I2C_START;
                    log_to_vcom("i2c master sends data timeout in WRITE!\n");
                }
            }
            timeout = 0;
            state = I2C_STOP;
            break;
        case I2C_STOP:
            /* send a stop condition to I2C bus */
            i2c_stop_on_bus(AT24_I2CX);
            /* i2c master sends STOP signal successfully */
            while((I2C_CTL0(AT24_I2CX) & I2C_CTL0_STOP) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_END;
                i2c_timeout_flag = I2C_OK;
            } else {
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c master sends stop signal timeout in WRITE!\n");
            }
            break;
        default:
            state = I2C_START;
            i2c_timeout_flag = I2C_OK;
            timeout = 0;
            log_to_vcom("i2c master sends start signal in WRITE.\n");
            break;
        }
    }
    return I2C_END;
}

/*!
    \brief      write buffer of data to the EEPROM use timeout function
    \param[in]  p_buffer: pointer to the buffer  containing the data to be written to the EEPROM
    \param[in]  write_address: EEPROM's internal address to write to
    \param[in]  number_of_byte: number of bytes to write to the EEPROM
    \param[out] none
    \retval     none
*/
void eeprom_buffer_write_timeout(uint8_t *p_buffer, uint8_t write_address, uint16_t number_of_byte)
{
    uint8_t number_of_page = 0, number_of_single = 0, address = 0, count = 0;

    address = write_address % I2C_PAGE_SIZE;
    count = I2C_PAGE_SIZE - address;
    number_of_page = number_of_byte / I2C_PAGE_SIZE;
    number_of_single = number_of_byte % I2C_PAGE_SIZE;

    /* if write_address is I2C_PAGE_SIZE aligned  */
    if(0 == address) {
        while(number_of_page--) {
            eeprom_page_write_timeout(p_buffer, write_address, I2C_PAGE_SIZE);
            eeprom_wait_standby_state_timeout();
            write_address += I2C_PAGE_SIZE;
            p_buffer += I2C_PAGE_SIZE;
        }
        if(0 != number_of_single) {
            eeprom_page_write_timeout(p_buffer, write_address, number_of_single);
            eeprom_wait_standby_state_timeout();
        }
    } else {
        /* if write_address is not I2C_PAGE_SIZE aligned */
        if(number_of_byte < count) {
            eeprom_page_write_timeout(p_buffer, write_address, number_of_byte);
            eeprom_wait_standby_state_timeout();
        } else {
            number_of_byte -= count;
            number_of_page = number_of_byte / I2C_PAGE_SIZE;
            number_of_single = number_of_byte % I2C_PAGE_SIZE;

            if(0 != count) {
                eeprom_page_write_timeout(p_buffer, write_address, count);
                eeprom_wait_standby_state_timeout();
                write_address += count;
                p_buffer += count;
            }
            /* write page */
            while(number_of_page--) {
                eeprom_page_write_timeout(p_buffer, write_address, I2C_PAGE_SIZE);
                eeprom_wait_standby_state_timeout();
                write_address += I2C_PAGE_SIZE;
                p_buffer += I2C_PAGE_SIZE;
            }
            /* write single */
            if(0 != number_of_single) {
                eeprom_page_write_timeout(p_buffer, write_address, number_of_single);
                eeprom_wait_standby_state_timeout();
            }
        }
    }
}

/*!
    \brief      read data from the EEPROM
    \param[in]  p_buffer: pointer to the buffer that receives the data read from the EEPROM
    \param[in]  read_address: EEPROM's internal address to start reading from
    \param[in]  number_of_byte: number of bytes to reads from the EEPROM
    \param[out] none
    \retval     none
*/
uint8_t eeprom_buffer_read_timeout(uint8_t *p_buffer, uint8_t read_address, uint16_t number_of_byte)
{
    uint8_t   state = I2C_START;
    uint8_t   read_cycle = 0;
    uint16_t  timeout = 0;
    uint8_t   i2c_timeout_flag = 0;

    /* enable acknowledge */
    i2c_ack_config(AT24_I2CX, I2C_ACK_ENABLE);
    while(!(i2c_timeout_flag)) {
        switch(state) {
        case I2C_START:
            if(RESET == read_cycle) {
                /* i2c master sends start signal only when the bus is idle */
                while(i2c_flag_get(AT24_I2CX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    /* whether to send ACK or not for the next byte */
                    if(2 == number_of_byte) {
                        i2c_ackpos_config(AT24_I2CX, I2C_ACKPOS_NEXT);
                    }
                } else {
                    at24_i2c_bus_reset();
                    timeout = 0;
                    state = I2C_START;
                    log_to_vcom("i2c bus is busy in READ!\n");
                }
            }
            /* send the start signal */
            i2c_start_on_bus(AT24_I2CX);
            timeout = 0;
            state = I2C_SEND_ADDRESS;
            break;
        case I2C_SEND_ADDRESS:
            /* i2c master sends START signal successfully */
            while((! i2c_flag_get(AT24_I2CX, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                if(RESET == read_cycle) {
                    i2c_master_addressing(AT24_I2CX, eeprom_address, I2C_TRANSMITTER);
                    state = I2C_CLEAR_ADDRESS_FLAG;
                } else {
                    i2c_master_addressing(AT24_I2CX, eeprom_address, I2C_RECEIVER);
                    if(number_of_byte < 3) {
                        /* disable acknowledge */
                        i2c_ack_config(AT24_I2CX, I2C_ACK_DISABLE);
                    }
                    state = I2C_CLEAR_ADDRESS_FLAG;
                }
                timeout = 0;
            } else {
                timeout = 0;
                state = I2C_START;
                read_cycle = 0;
                log_to_vcom("i2c master sends start signal timeout in READ!\n");
            }
            break;
        case I2C_CLEAR_ADDRESS_FLAG:
            /* address flag set means i2c slave sends ACK */
            while((!i2c_flag_get(AT24_I2CX, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_flag_clear(AT24_I2CX, I2C_FLAG_ADDSEND);
                if((SET == read_cycle) && (1 == number_of_byte)) {
                    /* send a stop condition to I2C bus */
                    i2c_stop_on_bus(AT24_I2CX);
                }
                timeout = 0;
                state   = I2C_TRANSMIT_DATA;
            } else {
                timeout = 0;
                state   = I2C_START;
                read_cycle = 0;
                log_to_vcom("i2c master clears address flag timeout in READ!\n");
            }
            break;
        case I2C_TRANSMIT_DATA:
            if(RESET == read_cycle) {
                /* wait until the transmit data buffer is empty */
                while((! i2c_flag_get(AT24_I2CX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    /* send the EEPROM's internal address to write to : only one byte address */
                    i2c_data_transmit(AT24_I2CX, read_address);
                    timeout = 0;
                } else {
                    timeout = 0;
                    state = I2C_START;
                    read_cycle = 0;
                    log_to_vcom("i2c master wait data buffer is empty timeout in READ!\n");
                }
                /* wait until BTC bit is set */
                while((!i2c_flag_get(AT24_I2CX, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    timeout = 0;
                    state = I2C_START;
                    read_cycle++;
                } else {
                    timeout = 0;
                    state = I2C_START;
                    read_cycle = 0;
                    log_to_vcom("i2c master sends EEPROM's internal address timeout in READ!\n");
                }
            } else {
                while(number_of_byte) {
                    timeout++;
                    if(3 == number_of_byte) {
                        /* wait until BTC bit is set */
                        while(!i2c_flag_get(AT24_I2CX, I2C_FLAG_BTC));
                        /* disable acknowledge */
                        i2c_ack_config(AT24_I2CX, I2C_ACK_DISABLE);
                    }
                    if(2 == number_of_byte) {
                        /* wait until BTC bit is set */
                        while(!i2c_flag_get(AT24_I2CX, I2C_FLAG_BTC));
                        /* send a stop condition to I2C bus */
                        i2c_stop_on_bus(AT24_I2CX);
                    }
                    /* wait until RBNE bit is set */
                    if(i2c_flag_get(AT24_I2CX, I2C_FLAG_RBNE)) {
                        /* read a byte from the EEPROM */
                        *p_buffer = i2c_data_receive(AT24_I2CX);

                        /* point to the next location where the byte read will be saved */
                        p_buffer++;

                        /* decrement the read bytes counter */
                        number_of_byte--;
                        timeout = 0;
                    }
                    if(timeout > I2C_TIME_OUT) {
                        timeout = 0;
                        state = I2C_START;
                        read_cycle = 0;
                        log_to_vcom("i2c master sends data timeout in READ!\n");
                    }
                }
                timeout = 0;
                state = I2C_STOP;
            }
            break;
        case I2C_STOP:
            /* i2c master sends STOP signal successfully */
            while((I2C_CTL0(AT24_I2CX) & I2C_CTL0_STOP) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_END;
                i2c_timeout_flag = I2C_OK;
            } else {
                timeout = 0;
                state = I2C_START;
                read_cycle = 0;
                log_to_vcom("i2c master sends stop signal timeout in READ!\n");
            }
            break;
        default:
            state = I2C_START;
            read_cycle = 0;
            i2c_timeout_flag = I2C_OK;
            timeout = 0;
            log_to_vcom("i2c master sends start signal in READ.\n");
            break;
        }
    }
    return I2C_END;
}

/*!
    \brief      wait for EEPROM standby state use timeout function
    \param[in]  none
    \param[out] none
    \retval     none
*/
uint8_t eeprom_wait_standby_state_timeout()
{
    uint8_t   state = I2C_START;
    uint16_t  timeout = 0;
    uint8_t   i2c_timeout_flag = 0;

    while(!(i2c_timeout_flag)) {
        switch(state) {
        case I2C_START:
            /* i2c master sends start signal only when the bus is idle */
            while(i2c_flag_get(AT24_I2CX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_start_on_bus(AT24_I2CX);
                timeout = 0;
                state = I2C_SEND_ADDRESS;
            } else {
                at24_i2c_bus_reset();
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c bus is busy in EEPROM standby!\n");
            }
            break;
        case I2C_SEND_ADDRESS:
            /* i2c master sends START signal successfully */
            while((! i2c_flag_get(AT24_I2CX, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_master_addressing(AT24_I2CX, eeprom_address, I2C_TRANSMITTER);
                timeout = 0;
                state = I2C_CLEAR_ADDRESS_FLAG;
            } else {
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c master sends start signal timeout in EEPROM standby!\n");
            }
            break;
        case I2C_CLEAR_ADDRESS_FLAG:
            while((!((i2c_flag_get(AT24_I2CX, I2C_FLAG_ADDSEND)) || (i2c_flag_get(AT24_I2CX, I2C_FLAG_AERR)))) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                if(i2c_flag_get(AT24_I2CX, I2C_FLAG_ADDSEND)) {
                    i2c_flag_clear(AT24_I2CX, I2C_FLAG_ADDSEND);
                    timeout = 0;
                    /* send a stop condition to I2C bus */
                    i2c_stop_on_bus(AT24_I2CX);
                    i2c_timeout_flag = I2C_OK;
                    /* exit the function */
                    return I2C_END;
                } else {
                    /* clear the bit of AE */
                    i2c_flag_clear(AT24_I2CX, I2C_FLAG_AERR);
                    timeout = 0;
                    state = I2C_STOP;
                }
            } else {
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c master clears address flag timeout in EEPROM standby!\n");
            }
            break;
        case I2C_STOP:
            /* send a stop condition to I2C bus */
            i2c_stop_on_bus(AT24_I2CX);
            /* i2c master sends STOP signal successfully */
            while((I2C_CTL0(AT24_I2CX) & I2C_CTL0_STOP) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_START;
            } else {
                timeout = 0;
                state = I2C_START;
                log_to_vcom("i2c master sends stop signal timeout in EEPROM standby!\n");
            }
            break;
        default:
            state = I2C_START;
            timeout = 0;
            log_to_vcom("i2c master sends start signal end in EEPROM standby!.\n");
            break;
        }
    }
    return I2C_END;

}
