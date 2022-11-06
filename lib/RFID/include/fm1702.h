#ifndef __fm1702_h
#define __fm1702_h

#include "board_conf.h"
#include "spi_fm1702.h"


//;==============================================
//;FM1702SL???????
//;==============================================
#define	Page_Reg                0x00	//?:0x80  ?:0x00
#define	Command_Reg             0x01	//?:0x82	?:0x02
#define	FIFO_Reg                0x02	//?:0x84	?:0x04
#define	FIFOLength_Reg		    0x04	//?:0x88
#define	SecondaryStatus_Reg  	0x05	//?:0x8A
#define	InterruptEn_Reg		    0x06	//			?:0x0C
#define	InterruptRq_Reg		    0x07	//?:0x8E	?:0x0E
#define	Control_Reg		        0x09	//?:0x92	?:0x12 
#define	ErrorFlag_Reg           0x0A	//?:0x94
#define	BitFraming_Reg	     	0x0F	//			?:0x1E
#define	TxControl_Reg           0x11	//			?:0x22
#define	CwConductance_Reg	    0x12
#define	RxControl2_Reg		    0x1E
#define RxWait_Reg              0x21
#define	ChannelRedundancy_Reg	0x22
 #define CRCPresetLSB        0x23
    #define CRCPresetMSB        0x24
    #define MFOUTSelect         0x26   
    #define TimerClock          0x2a   
    #define TimerControl        0x2b   
    #define TimerReload         0x2c    
    #define TypeSH              0x31   
    #define TestDigiSelect      0x3d 

//;==============================================
//;????????
//;==============================================
#define FM1702_OK			0		// ??
#define FM1702_NOTAGERR		1		// ??
#define FM1702_CRCERR		2		// ??CRC????
#define FM1702_EMPTY		3		// ??????
#define FM1702_AUTHERR		4		// ?????
#define FM1702_PARITYERR    5		// ????????
#define FM1702_CODEERR		6		// ????(BCC???)
#define FM1702_SERNRERR		8		// ???????(anti-collision ??)
#define FM1702_SELECTERR    9		// ??????????(SELECT??)
#define FM1702_NOTAUTHERR	10		// 0x0A ????????
#define FM1702_BITCOUNTERR	11		// ???????????
#define FM1702_BYTECOUNTERR	12		// ??????????????????
#define FM1702_RESTERR		13		// ??restore????
#define FM1702_TRANSERR		14		// ??transfer????
#define FM1702_WRITEERR		15		// 0x0F ??write????
#define FM1702_INCRERR		16		// 0x10 ??increment????
#define FM1702_DECRERR		17      // 0x11 ??decrement????
#define FM1702_READERR		18      // 0x12 ??read????
#define FM1702_LOADKEYERR	19      // 0x13 ??LOADKEY????
#define FM1702_FRAMINGERR	20      // 0x14 FM1702???
#define FM1702_REQERR		21      // 0x15 ??req????
#define FM1702_SELERR		22      // 0x16 ??sel????
#define FM1702_ANTICOLLERR	23      // 0x17 ??anticoll????
#define FM1702_INTIVALERR	24      // 0x18 ?????????
#define FM1702_READVALERR	25      // 0x19 ???????????
#define FM1702_DESELECTERR	26      // 0x1A
#define FM1702_CMD_ERR		42      // 0x2A ????

//;==============================================
//;??????????
//;==============================================
#define RF_CMD_REQUEST_STD	0x26
#define RF_CMD_REQUEST_ALL	0x52
#define RF_CMD_ANTICOL		0x93
#define RF_CMD_SELECT		0x93
#define RF_CMD_AUTH_LA		0x60
#define RF_CMD_AUTH_LB		0x61
#define RF_CMD_READ         0x30
#define RF_CMD_WRITE		0xa0
#define RF_CMD_INC		    0xc1
#define RF_CMD_DEC		    0xc0
#define RF_CMD_RESTORE		0xc2
#define RF_CMD_TRANSFER		0xb0
#define RF_CMD_HALT		    0x50


extern uint8_t gBuff[16];             //M1?????????
extern uint8_t gCard_UID[5];	//4?????(32?),??????

////===============================================
//???FM1702??????
//===============================================
void drv_fm1702PowerDown(void);
bool drv_fm1702Init(void);
uint8_t drv_fm1702ReadFifo(uint8_t *buff);
uint8_t drv_fm1702Command(uint8_t Comm_Set, uint8_t *buff, uint8_t count);
uint8_t drv_fm1702Request(uint8_t mode);
uint8_t drv_fm1702AntiColl(void);
uint8_t drv_fm1702SelectCard(void);
uint8_t drv_fm1702LoadKey(uint8_t *ramadr);
uint8_t Authentication(uint8_t *UID, uint8_t SecNR, uint8_t mode);
uint8_t drv_fm1702ReadBlock(uint8_t *buff, uint8_t index);
uint8_t drv_fm1702WriteBlock(uint8_t *buff, uint8_t index);
uint8_t drv_fm1702HaltCard(void);


#endif