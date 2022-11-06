#include "fm1702.h"
#include "stdio.h"


////////////////////????////////////////////////////////
uint8_t gBuff[16];
uint8_t gCard_UID[5];

static void drv_fm1702WriteFifo(uint8_t *buff, uint8_t count);
static bool drv_fm1702ClearFifo(void);

//;==============================================
//;FM1702SL??????
//;==============================================
#define CMD_WRITE_E2            0x01
#define CMD_LOAD_KEY_E2         0x0B
#define CMD_LOAD_KEY            0x19
#define CMD_TRANSMIT            0x1A
#define CMD_TRANSCCEIVE         0x1E
#define CMD_AUTHENT_1           0x0C
#define CMD_AUTHENT_2           0x14


//=======================================================
//  ??: drv_fm1702PowerDownMode
//  ??: ?????FM1702?????????,
//      ?????????
//  ??:N/A
//  ??: N/A
//=======================================================
void drv_fm1702PowerDownMode(void)
{
    drv_fm1702ItfSetReset();
}


//=======================================================
//  ??: drv_fm1702Init
//  ??: ??????FM1702?????
//  ??:N/A
//  ??: N/A
//=======================================================
bool drv_fm1702Init(void)
{
    uint8_t temp;
    uint16_t i = 100;
    drv_fm1702ItfGpioInit();

    drv_fm1702ItfSetReset();    // FM1702??
    delay_1ms(10);
    drv_fm1702ItfClearReset();
    //??????,??512?FM1702SL?????,?38us
    delay_1ms(10);
    //???????,??128?????,?10us

    // ??Command = 0,FM1702????
    for (i = 0; i < 0x1fff; i++) // ??
    {
        temp = drv_fm1702ItfReadByte(Command_Reg);
        if(temp == 0x00)
        {
            break;
        }
        if(i >= 0xFF)
        {
            return FALSE;
        }
				
				 delay_1ms(100);
    }
    ///////////////////////////////////////////////////////////////////////////////////
    drv_fm1702ItfWriteByte(Page_Reg, 0x80); //???SPI??
    for (i = 0; i < 0x1FFF; i++) // ??
    {
        temp = drv_fm1702ItfReadByte(Command_Reg);
        if (temp == 0x00)   //SPI?????
        {
            drv_fm1702ItfWriteByte(Page_Reg, 0);//????SPI??
            break;
        }
        if( i >= 0x1FFF)
        {
            return FALSE;
        }
				
				delay_1ms(100);
    }
    ////////////////////////////////////////////////////////////////////////////////////
    drv_fm1702ItfWriteByte(InterruptEn_Reg, 0x7F);  //  ????????(????0)
    drv_fm1702ItfWriteByte(InterruptRq_Reg, 0x7F);  // ???????????0(????0)

    //???????????????, ????TX1?TX2
    drv_fm1702ItfWriteByte(TxControl_Reg, 0x5B);        // ???????
    drv_fm1702ItfWriteByte(RxControl2_Reg, 0x01);
    drv_fm1702ItfWriteByte(RxWait_Reg, 5);
	
    //??TX1?TX2???????
    drv_fm1702ItfWriteByte(CwConductance_Reg, 0x3F);
	
    return TRUE;
}

//=======================================================
//  ??: drv_fm1702Request
//  ??:   ????????FM1702??????????Request??
//          ?:??M1??????
//  ??:   mode: ALL(????FM1702?????????)
//          STD(???FM1702????????HALT?????)
//          Comm_Set, ???:?FM1702??IC????
//  ??:   FM1702_NOTAGERR: ??
//          FM1702_OK: ????
//          FM1702_REQERR: ????
//=======================================================
uint8_t drv_fm1702Request(uint8_t mode)
{
    uint8_t temp;
	
		//drv_fm1702ItfWriteByte(TxControl_Reg, 0x58); 
    //delay_1ms(1);
		//drv_fm1702ItfWriteByte(CRCPresetLSB, 0x63); 
		//drv_fm1702ItfWriteByte(CwConductance_Reg, 0x3F); 
	
    drv_fm1702ItfWriteByte(ChannelRedundancy_Reg, 0x03);
    //????bit???
    drv_fm1702ItfWriteByte(BitFraming_Reg, 0x07);
    gBuff[0] = mode;        //Request????
		//drv_fm1702ItfWriteByte(TxControl_Reg, 0x5B);
    temp = drv_fm1702ItfReadByte(Control_Reg);
    temp = temp & (0xf7);
    drv_fm1702ItfWriteByte(Control_Reg, temp);        //Control reset value is 00
    temp = drv_fm1702Command(CMD_TRANSCCEIVE, gBuff, 1);   //??????
    if (temp == 0)
    {
        return FM1702_NOTAGERR;
    }

    temp = drv_fm1702ReadFifo(gBuff);       //?FIFO????????gBuff?
    // ??????????
    //2     Mifare Pro ?
    //4     Mifare One ?
    if ((gBuff[0] == 0x04) & (gBuff[1] == 0x0) & (temp == 2))
    {
        return FM1702_OK;
    }
    return FM1702_REQERR;
}

//=======================================================
//??:drv_fm1702ClearFifo
//??:??FM1702?FIFO
//??:
//??: TRUE:??,FALSE:??
//========================================================
static bool drv_fm1702ClearFifo(void)
{
    uint8_t ucResult, i;
    ucResult = drv_fm1702ItfReadByte(Control_Reg);
    ucResult |= 0x01;
    drv_fm1702ItfWriteByte(Control_Reg, ucResult);
    for (i = 0; i < 0xA0; i++)
    {
        ucResult = drv_fm1702ItfReadByte(FIFOLength_Reg);
        if (ucResult == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

//=======================================================
//??:drv_fm1702WriteFifo
//??:?????FM1702?FIFO
//??: buff:???????Ram?????
//      count:???????
//??:
//========================================================
static void drv_fm1702WriteFifo(uint8_t *buff, uint8_t count)
{
    drv_fm1702ItfWrite(FIFO_Reg, buff, count);
}

//=======================================================
//  ??: drv_fm1702ReadFifo
//  ??: ??????FM1702?FIFO???x bytes??
//  ??:   buff, ?????????
//  ??:   ????(??:??)
//=======================================================
uint8_t drv_fm1702ReadFifo(uint8_t *buff)
{
    uint8_t ucResult;

    ucResult = drv_fm1702ItfReadByte(FIFOLength_Reg);
    if (ucResult == 0 || ucResult > 16)
    {
        return 0;
    }
    drv_fm1702ItfRead(FIFO_Reg, buff, ucResult);

    return ucResult;
}

//=======================================================
//  ??: drv_fm1702Command
//  ??: ??????FM1702????????
//  ??:   count, ?????????
//          buff, ??????????
//          Comm_Set, ???:?FM1702??IC????
//  ??:   TRUE, ???????
//          FALSE, ??????
//=======================================================
uint8_t drv_fm1702Command(uint8_t Comm_Set, uint8_t *buff, uint8_t count)
{
    uint8_t ucResult1, ucResult2, i;
    drv_fm1702ItfWriteByte(Command_Reg, 0x00);          //???IDLE
    if (drv_fm1702ClearFifo() == FALSE)
    {
        return FALSE;
    }
    drv_fm1702WriteFifo(buff, count);
    drv_fm1702ItfWriteByte(Command_Reg, Comm_Set);
    for (i = 0; i < 0xA0; i++)
    {
        ucResult1 = drv_fm1702ItfReadByte(Command_Reg);
        ucResult2 = drv_fm1702ItfReadByte(InterruptRq_Reg) & 0x80;
        if (ucResult1 == 0 || ucResult2 == 0x80)
        {
            return TRUE;
        }
    }
    return FALSE;
}

//=======================================================
//  ??: drv_fm1702AntiColl
//  ??:   ????????FM1702???????????????
//  ??:   ????ID(6Byte)
//  ??:   FM1702_NOTAGERR: ??
//          FM1702_OK: ????
//          FM1702_BYTECOUNTERR: ??????
//          FM1702_SERNRERR: ?????????
//=======================================================
uint8_t drv_fm1702AntiColl(void)
{
    uint8_t temp, i;

    //???????????
    drv_fm1702ItfWriteByte(ChannelRedundancy_Reg, 0x03);

    gBuff[0] = RF_CMD_ANTICOL;
    gBuff[1] = 0x20;
    temp = drv_fm1702Command(CMD_TRANSCCEIVE, gBuff, 2);
    if (temp == 0)
    {
        return FM1702_NOTAGERR;
    }

    temp = drv_fm1702ItfReadByte(FIFOLength_Reg);
    if (temp == 0)
    {
        return FM1702_BYTECOUNTERR;
    }

    drv_fm1702ItfRead(FIFO_Reg, gBuff, temp);

    temp = drv_fm1702ItfReadByte(ErrorFlag_Reg);    // ????????????
    temp = temp & 0x01;
    if (temp == 0x00)
    {
        for (i = 0; i < 5; i++)
        {
            temp ^= gBuff[i];
        }
        if (temp)
        {
            return FM1702_SERNRERR;
        }
        for (i = 0; i < 5; i++)
        {
            gCard_UID[i] = gBuff[i];
        }
        return FM1702_OK;
    }
    else //????
    {
        return FM1702_SERNRERR;
    }
}

//=======================================================
//  ??: drv_fm1702SelectCard
//  ??:   ????????FM1702???????????????
//  ??:   N/A
//  ??:   FM1702_NOTAGERR: ??
//          FM1702_OK: ????
//          FM1702_BYTECOUNTERR: ??????
//          FM1702_PARITYERR: ?????
//          FM1702_CRCERR: CRC???
//          FM1702_SELERR: ????
//=======================================================
uint8_t drv_fm1702SelectCard(void)
{
    uint8_t   temp, i;

    //???????????
    drv_fm1702ItfWriteByte(ChannelRedundancy_Reg, 0x0F);

    gBuff[0] = RF_CMD_SELECT;
    gBuff[1] = 0x70;
    for (i = 0; i < 5; i++)
    {
        gBuff[i + 2] = gCard_UID[i];
    }
    temp = drv_fm1702Command(CMD_TRANSCCEIVE, gBuff, 7);
    if (temp == 0)
    {
        return(FM1702_NOTAGERR);
    }
    else
    {
        temp = drv_fm1702ItfReadByte(ErrorFlag_Reg);
        if (temp & 0x02)
        {
            return FM1702_PARITYERR;
        }
        if (temp & 0x04)
        {
            return FM1702_FRAMINGERR;
        }
        if (temp & 0x08)
        {
            return FM1702_CRCERR;
        }
        temp = drv_fm1702ItfReadByte(FIFOLength_Reg);
        if (temp != 0x01)
        {
            return FM1702_BYTECOUNTERR;
        }

        temp = drv_fm1702ItfReadByte(FIFO_Reg);     //?FIFO???????
        if (temp == 0x08)                           // ??????????
        {
            return FM1702_OK;
        }
        else
        {
            return FM1702_SELERR;
        }
    }
}

uint8_t drv_fm1702LoadKey(uint8_t *ramadr)
{
    uint8_t acktemp, temp, i;
    uint8_t u8_Buff[12];
    for (i = 0; i < 6; i++)
    {
        temp = ramadr[i];
        u8_Buff[2 * i] = (((ramadr[i] & 0xF0) >> 4) | ((~ramadr[i]) & 0xF0));
        u8_Buff[2 * i + 1] = ((temp & 0x0F) | (~(temp & 0x0F) << 4));
    }
    acktemp = drv_fm1702Command(CMD_LOAD_KEY, u8_Buff, 12);
    temp = drv_fm1702ItfReadByte(ErrorFlag_Reg);
    if (temp & 0x40)
    {
        temp = 0x0;
        drv_fm1702ItfWriteByte(Command_Reg, temp);
        return 0;
    }
    temp = 0x0;
    drv_fm1702ItfWriteByte(Command_Reg, temp);
    return 1;
}

//=======================================================
//  ??: drv_fm1702LoadKeyToE2
//  ??: ??????????FM1702?E2?
//          ?????? =0x80+n*12
//  ??:   index: ????(??????,0~15?A??,16~31?B??)
//  ??:   TRUE, ??????
//          FALSE, ??????
//=======================================================
uint8_t drv_fm1702LoadKeyToE2(uint8_t index, uint8_t *ramadr)
{
    uint8_t acktemp, temp, i;
    uint8_t u8_Buff[16];
    uint16_t u16_Data;
    u16_Data = 0x80 + index * 12;
    u8_Buff[0] = (uint8_t)(u16_Data & 0x00FF);
    u8_Buff[1] = (uint8_t)(u16_Data >> 8);
    for (i = 0; i < 6; i++)
    {
        temp = ramadr[i];
        u8_Buff[2 + i + i] = (((ramadr[i] & 0xF0) >> 4) | ((~ramadr[i]) & 0xF0));
        u8_Buff[3 + i + i] = ((temp & 0x0F) | (~(temp & 0x0F) << 4));
    }
    acktemp = drv_fm1702Command(CMD_WRITE_E2, u8_Buff, 0x0e);
    delay_1ms(4);
    temp = drv_fm1702ItfReadByte(SecondaryStatus_Reg);
    if (temp & 0x40)
    {
        temp = 0x0;
        drv_fm1702ItfWriteByte(Command_Reg, temp);
        return FALSE;
    }
    temp = 0x0;
    drv_fm1702ItfWriteByte(Command_Reg, temp);
    return TRUE;
}


//=======================================================
//  ??: drv_fm1702LoadKeyfromE2
//  ??: ??????E2?????FM1702?keyRevBuffer?
//  ??:   index: ????(??????,0~15?A??,16~31?B??)
//  ??:   TRUE, ??????
//          FALSE, ??????
//=======================================================
uint8_t drv_fm1702LoadKeyfromE2(uint8_t index)
{
    uint8_t u8_Data[2], ucResult;
    uint16_t u16_Data;
    if(index >= 32)
    {
        return FALSE;
    }
    u16_Data = 0x80 + index * 12;
    u8_Data[0] = (uint8_t)(u16_Data & 0x00FF);
    u8_Data[1] = (uint8_t)(u16_Data >> 8);
    ucResult = drv_fm1702Command(CMD_LOAD_KEY_E2, u8_Data, 2);
    if(ucResult == FALSE)
    {
        return FALSE;
    }
    delay_1ms(1);
    ucResult = drv_fm1702ItfReadByte(ErrorFlag_Reg);
    if(ucResult & 0x40)
    {
        return FALSE;
    }
    return TRUE;
}

//=======================================================
//  ??: Authentication
//  ??:   ????????????
//  ??:   UID: ???????
//          SecNR: ???
//          mode: ??
//  ??:   FM1702_NOTAGERR: ??
//          FM1702_OK: ????
//          FM1702_PARITYERR: ?????
//          FM1702_CRCERR: CRC???
//          FM1702_AUTHERR: ??????
//=======================================================
uint8_t Authentication(uint8_t *UID, uint8_t SecNR, uint8_t mode)
{
    uint8_t i;
    uint8_t temp, temp1;

    if (SecNR >= 16)
    {
        SecNR = SecNR % 16;
    }

    //???????????
    drv_fm1702ItfWriteByte(ChannelRedundancy_Reg, 0x0F);
    gBuff[0] = mode;
    gBuff[1] = SecNR * 4 + 3;
    for (i = 0; i < 4; i++)
    {
        gBuff[2 + i] = UID[i];
    }

    temp = drv_fm1702Command(CMD_AUTHENT_1, gBuff, 6);
    if (temp == 0)
    {
        return 0x99;
    }

    temp = drv_fm1702ItfReadByte(ErrorFlag_Reg);
    if ((temp & 0x02) == 0x02) return FM1702_PARITYERR;
    if ((temp & 0x04) == 0x04) return FM1702_FRAMINGERR;
    if ((temp & 0x08) == 0x08) return FM1702_CRCERR;
    temp = drv_fm1702Command(CMD_AUTHENT_2, gBuff, 0);
    if (temp == 0)
    {
        return 0x88;
    }

    temp = drv_fm1702ItfReadByte(ErrorFlag_Reg);
    //  Show(temp,0);
    if ((temp & 0x02) == 0x02) return FM1702_PARITYERR;
    if ((temp & 0x04) == 0x04) return FM1702_FRAMINGERR;
    if ((temp & 0x08) == 0x08) return FM1702_CRCERR;
    temp1 = drv_fm1702ItfReadByte(Control_Reg);
    temp1 = temp1 & 0x08;
    if (temp1 == 0x08)
    {
        return FM1702_OK;
    }

    return FM1702_AUTHERR;
}

//=======================================================
//  ??: drv_fm1702ReadBlock
//  ??:   ??????MIFARE?????
//  ??:   buff: ??????
//          index: ???
//  ??:   FM1702_NOTAGERR: ??
//          FM1702_OK: ????
//          FM1702_PARITYERR: ?????
//          FM1702_CRCERR: CRC???
//          FM1702_BYTECOUNTERR: ??????
//=======================================================

uint8_t drv_fm1702ReadBlock(uint8_t *buff, uint8_t index)
{
    uint8_t ucCmdLine[2], ucResult;
    drv_fm1702ItfWriteByte(ChannelRedundancy_Reg, 0x0F);
    ucCmdLine[0] = RF_CMD_READ;
    ucCmdLine[1] = index;
    ucResult = drv_fm1702Command(CMD_TRANSCCEIVE, ucCmdLine, 2);
    if (ucResult == FALSE)
    {
        return FM1702_NOTAGERR;   //??
    }
    ucResult = drv_fm1702ItfReadByte(ErrorFlag_Reg);
    if ((ucResult & 0x02) == 0x02)
    {
        return FM1702_PARITYERR;
    }
    if ((ucResult & 0x04) == 0x04)
    {
        return FM1702_FRAMINGERR;
    }
    if ((ucResult & 0x08) == 0x08)
    {
        return FM1702_CRCERR;
    }
    ucResult = drv_fm1702ReadFifo(buff);
    if (ucResult != 0x10)
    {
        return FM1702_BYTECOUNTERR;
    }
    else
    {
        return FM1702_OK;
    }
}

//=======================================================
//  ??: drv_fm1702WriteBlock
//  ??:   ??????MIFARE?????
//  ??:   buff: ??????
//          index: ???
//  ??:   FM1702_NOTAGERR: ??
//          FM1702_NOTAUTHERR: ??????
//          FM1702_EMPTY: ??????
//          FM1702_WRITEERR: ???????
//          FM1702_OK: ????
//          FM1702_PARITYERR: ?????
//          FM1702_CRCERR: CRC???
//          FM1702_BYTECOUNTERR: ??????
//=======================================================
uint8_t drv_fm1702WriteBlock(uint8_t *buff, uint8_t index)
{
    uint8_t ucCmdLine[2], ucResult, u8_Data[16];
    drv_fm1702ItfWriteByte(ChannelRedundancy_Reg, 0x07);   /* Note: this line is for 1702, different from RC500*/
    ucCmdLine[0] = RF_CMD_WRITE;
    ucCmdLine[1] = index;
    ucResult = drv_fm1702Command(CMD_TRANSCCEIVE, ucCmdLine, 2);
    if (ucResult == FALSE)
        return FM1702_NOTAGERR;
    ucResult = drv_fm1702ReadFifo(u8_Data);
    if (ucResult == 0)
        return FM1702_BYTECOUNTERR;
    switch (u8_Data[0])
    {
        case 0x00:
            return(FM1702_NOTAUTHERR);
        case 0x04:
            return(FM1702_EMPTY);
        case 0x0a:
            break;                   //??
        case 0x01:
            return(FM1702_CRCERR);
        case 0x05:
            return(FM1702_PARITYERR);
        default:
            return(FM1702_WRITEERR);
    }
    ucResult = drv_fm1702Command(CMD_TRANSCCEIVE, buff, 16);
    if (ucResult == TRUE)
    {
        return FM1702_OK;
    }
    else
    {
        ucResult = drv_fm1702ItfReadByte(ErrorFlag_Reg);
        if ((ucResult & 0x02) == 0x02) return FM1702_PARITYERR;
        else if ((ucResult & 0x04) == 0x04) return FM1702_FRAMINGERR;
        else if ((ucResult & 0x08) == 0x08) return FM1702_CRCERR;
        else return FM1702_WRITEERR;
    }
}

//=======================================================
//  ??: drv_fm1702HaltCard
//  ??: ???????MIFARE?
//  ??:   N/A
//  ??:   FM1702_OK: ????
//          FM1702_PARITYERR: ?????
//          FM1702_FRAMINGERR:FM1702???
//          FM1702_CRCERR: CRC???
//          FM1702_NOTAGERR: ??
//=======================================================
uint8_t drv_fm1702HaltCard(void)
{
    uint8_t temp;
    //???????????
    drv_fm1702ItfWriteByte(ChannelRedundancy_Reg, 0x03);
    *gBuff = RF_CMD_HALT;
    *(gBuff + 1) = 0x00;
    temp = drv_fm1702Command(CMD_TRANSMIT, gBuff, 2);//??FIFO????
    if (temp == TRUE)
    {
        return FM1702_OK;
    }
    else
    {
        temp = drv_fm1702ItfReadByte(ErrorFlag_Reg);
        if ((temp & 0x02) == 0x02)
        {
            return(FM1702_PARITYERR);
        }

        if ((temp & 0x04) == 0x04)
        {
            return(FM1702_FRAMINGERR);
        }
        return(FM1702_NOTAGERR);
    }
}
