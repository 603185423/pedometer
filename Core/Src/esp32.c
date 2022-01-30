#include "esp32.h"
#include <usart.h>
//#define ESP32_BT_DEBUG_USART2
#define ESP32_BT_DEBUG_OLED
#ifdef ESP32_BT_DEBUG_OLED
#include "oled.h"
#endif // ESP32_BT_DEBUG_OLED


enum Esp32ReceiveState_LBBSB { LBBSB_RECEIVE_START, LBBSB_RECEIVE_CR1, LBBSB_RECEIVE_LF1, LBBSB_RECEIVE_CR2, LBBSB_RECEIVE_LF2, LBBSB_RECEIVE_CR3, LBBSB_RECEIVE_LF3 }esp32ReceiveState_LBBSB;
enum Esp32ReceiveState_LBLBLBBSB { LBLBLBBSB_RECEIVE_START, LBLBLBBSB_RECEIVE_CR1, LBLBLBBSB_RECEIVE_LF1, LBLBLBBSB_RECEIVE_CR2, LBLBLBBSB_RECEIVE_LF2, LBLBLBBSB_RECEIVE_CR3, LBLBLBBSB_RECEIVE_LF3, LBLBLBBSB_RECEIVE_CR4, LBLBLBBSB_RECEIVE_LF4, LBLBLBBSB_RECEIVE_CR5, LBLBLBBSB_RECEIVE_LF5 }esp32ReceiveState_LBLBLBBSB;
enum Esp32ReceiveState_LBLBBSB { LBLBBSB_RECEIVE_START, LBLBBSB_RECEIVE_CR1, LBLBBSB_RECEIVE_LF1, LBLBBSB_RECEIVE_CR2, LBLBBSB_RECEIVE_LF2, LBLBBSB_RECEIVE_CR3, LBLBBSB_RECEIVE_LF3, LBLBBSB_RECEIVE_CR4, LBLBBSB_RECEIVE_LF4 }esp32ReceiveState_LBLBBSB;
enum Esp32ReceiveState_LBBSBBS { LBBSBBS_RECEIVE_START, LBBSBBS_RECEIVE_CR1, LBBSBBS_RECEIVE_LF1, LBBSBBS_RECEIVE_CR2, LBBSBBS_RECEIVE_LF2, LBBSBBS_RECEIVE_CR3, LBBSBBS_RECEIVE_LF3, LBBSBBS_RECEIVE_CR4, LBBSBBS_RECEIVE_LF4, LBBSBBS_RECEIVE_LAB }esp32ReceiveState_LBBSBBS;
enum Esp32ReceiveState_SB { SB_RECEIVE_START, SB_RECEIVE_CR1, SB_RECEIVE_LF1 }esp32ReceiveState_SB;
enum Esp32ReceiveState_BS { BS_RECEIVE_START, BS_RECEIVE_CR1, BS_RECEIVE_LF1, BS_RECEIVE_LAB }esp32ReceiveState_BS;//LAB:left angle brcket
enum Esp32ReceiveType { RXNONE, LBBSB, SB, BS, LBLBLBBSB, LBLBBSB, LBBSBBS }esp32ReceiveType;
enum Esp32ReceiveState { RECEIVE_OK, RECEIVE_BUSY }esp32ReceiveState;
enum Esp32WirelessUse esp32WirelessUse = USE_WIFI;

uint8_t rx1Buffer[1], txEndString[] = "\r\n";
uint8_t esp32RxBuffer[ESP32_RX_BUFFER_SIZE];
uint8_t *pEsp32RxBufferWrite = esp32RxBuffer;
uint8_t *pEsp32RxBufferRead = esp32RxBuffer;




uint8_t a = 0;


void Esp32BlockingReceive(void)
{
	while (esp32ReceiveState == RECEIVE_BUSY)
	{
		;
	}
	esp32ReceiveType = RXNONE;
#ifdef ESP32_BT_DEBUG_USART2
	HAL_UART_Transmit(&huart2, pEsp32RxBufferRead, pEsp32RxBufferWrite - pEsp32RxBufferRead, ESP32_TIMEOUT_LONGSENT);
#endif // ESP32_BT_DEBUG_USART2
#ifdef ESP32_BT_DEBUG_OLED
	OledDisplayLine(pEsp32RxBufferRead);
#endif // ESP32_BT_DEBUG_OLED

}

void Esp32RxCalback_LBBSB(void)
{
	if (rx1Buffer[0] == '\r')
	{
		if (esp32ReceiveState_LBBSB == LBBSB_RECEIVE_START) esp32ReceiveState_LBBSB = LBBSB_RECEIVE_CR1;
		else if (esp32ReceiveState_LBBSB == LBBSB_RECEIVE_LF1) esp32ReceiveState_LBBSB = LBBSB_RECEIVE_CR2;
		else if (esp32ReceiveState_LBBSB == LBBSB_RECEIVE_LF2) esp32ReceiveState_LBBSB = LBBSB_RECEIVE_CR3;
	}
	else if (rx1Buffer[0] == '\n')
	{
		if (esp32ReceiveState_LBBSB == LBBSB_RECEIVE_CR1) esp32ReceiveState_LBBSB = LBBSB_RECEIVE_LF1;
		else if (esp32ReceiveState_LBBSB == LBBSB_RECEIVE_CR2) esp32ReceiveState_LBBSB = LBBSB_RECEIVE_LF2;
		else if (esp32ReceiveState_LBBSB == LBBSB_RECEIVE_CR3) 
		{
			esp32ReceiveState_LBBSB = LBBSB_RECEIVE_LF3;
			*pEsp32RxBufferWrite++ = '\0';
			esp32ReceiveState = RECEIVE_OK;
		}
	}
	else
	{
		if (esp32ReceiveState_LBBSB == LBBSB_RECEIVE_LF2) *pEsp32RxBufferWrite++ = rx1Buffer[0];
	}
}

void Esp32RxCalback_SB(void)
{
	if (rx1Buffer[0] == '\r' && esp32ReceiveState_SB == SB_RECEIVE_START) esp32ReceiveState_SB = SB_RECEIVE_CR1;
	else if (rx1Buffer[0] == '\n' && esp32ReceiveState_SB == SB_RECEIVE_CR1)
	{
		esp32ReceiveState_SB = SB_RECEIVE_LF1;
		*pEsp32RxBufferWrite++ = '\0';
		esp32ReceiveState = RECEIVE_OK;
	}
	else
	{
		if (esp32ReceiveState_SB == SB_RECEIVE_START) *pEsp32RxBufferWrite++ = rx1Buffer[0];
	}
}

void Esp32RxCalback_BS(void)
{
	if (rx1Buffer[0] == '\r' && esp32ReceiveState_BS == BS_RECEIVE_START) esp32ReceiveState_BS = BS_RECEIVE_CR1;
	else if (rx1Buffer[0] == '\n' && esp32ReceiveState_BS == BS_RECEIVE_CR1) esp32ReceiveState_BS = BS_RECEIVE_LF1;
	else if (rx1Buffer[0] == '>' && esp32ReceiveState_BS == BS_RECEIVE_LF1)
	{
		esp32ReceiveState_BS = BS_RECEIVE_LAB;
		*pEsp32RxBufferWrite++ = rx1Buffer[0];
		*pEsp32RxBufferWrite++ = '\0';
		esp32ReceiveState = RECEIVE_OK;
	}
}

void Esp32RxCalback_LBLBLBBSB(void)
{
	if (rx1Buffer[0] == '\r')
	{
		if (esp32ReceiveState_LBLBLBBSB == LBLBLBBSB_RECEIVE_START) esp32ReceiveState_LBLBLBBSB = LBLBLBBSB_RECEIVE_CR1;
		else if (esp32ReceiveState_LBLBLBBSB == LBLBLBBSB_RECEIVE_LF1) esp32ReceiveState_LBLBLBBSB = LBLBLBBSB_RECEIVE_CR2;
		else if (esp32ReceiveState_LBLBLBBSB == LBLBLBBSB_RECEIVE_LF2) esp32ReceiveState_LBLBLBBSB = LBLBLBBSB_RECEIVE_CR3;
		else if (esp32ReceiveState_LBLBLBBSB == LBLBLBBSB_RECEIVE_LF3) esp32ReceiveState_LBLBLBBSB = LBLBLBBSB_RECEIVE_CR4;
		else if (esp32ReceiveState_LBLBLBBSB == LBLBLBBSB_RECEIVE_LF4) esp32ReceiveState_LBLBLBBSB = LBLBLBBSB_RECEIVE_CR5;
	}
	else if (rx1Buffer[0] == '\n')
	{
		if (esp32ReceiveState_LBLBLBBSB == LBLBLBBSB_RECEIVE_CR1) esp32ReceiveState_LBLBLBBSB = LBLBLBBSB_RECEIVE_LF1;
		else if (esp32ReceiveState_LBLBLBBSB == LBLBLBBSB_RECEIVE_CR2) esp32ReceiveState_LBLBLBBSB = LBLBLBBSB_RECEIVE_LF2;
		else if (esp32ReceiveState_LBLBLBBSB == LBLBLBBSB_RECEIVE_CR3) esp32ReceiveState_LBLBLBBSB = LBLBLBBSB_RECEIVE_LF3;
		else if (esp32ReceiveState_LBLBLBBSB == LBLBLBBSB_RECEIVE_CR4) esp32ReceiveState_LBLBLBBSB = LBLBLBBSB_RECEIVE_LF4;
		else if (esp32ReceiveState_LBLBLBBSB == LBLBLBBSB_RECEIVE_CR5) 
		{
			esp32ReceiveState_LBLBLBBSB = LBLBLBBSB_RECEIVE_LF5;
			*pEsp32RxBufferWrite++ = '\0';
			esp32ReceiveState = RECEIVE_OK;
		}
	}
	else
	{
		if (esp32ReceiveState_LBLBLBBSB == LBLBLBBSB_RECEIVE_LF4) *pEsp32RxBufferWrite++ = rx1Buffer[0];
	}
}
void Esp32RxCalback_LBLBBSB(void)
{
	if (rx1Buffer[0] == '\r')
	{
		if (esp32ReceiveState_LBLBBSB == LBLBBSB_RECEIVE_START) esp32ReceiveState_LBLBBSB = LBLBBSB_RECEIVE_CR1;
		else if (esp32ReceiveState_LBLBBSB == LBLBBSB_RECEIVE_LF1) esp32ReceiveState_LBLBBSB = LBLBBSB_RECEIVE_CR2;
		else if (esp32ReceiveState_LBLBBSB == LBLBBSB_RECEIVE_LF2) esp32ReceiveState_LBLBBSB = LBLBBSB_RECEIVE_CR3;
		else if (esp32ReceiveState_LBLBBSB == LBLBBSB_RECEIVE_LF3) esp32ReceiveState_LBLBBSB = LBLBBSB_RECEIVE_CR4;
	}
	else if (rx1Buffer[0] == '\n')
	{
		if (esp32ReceiveState_LBLBBSB == LBLBBSB_RECEIVE_CR1) esp32ReceiveState_LBLBBSB = LBLBBSB_RECEIVE_LF1;
		else if (esp32ReceiveState_LBLBBSB == LBLBBSB_RECEIVE_CR2) esp32ReceiveState_LBLBBSB = LBLBBSB_RECEIVE_LF2;
		else if (esp32ReceiveState_LBLBBSB == LBLBBSB_RECEIVE_CR3) esp32ReceiveState_LBLBBSB = LBLBBSB_RECEIVE_LF3;
		else if (esp32ReceiveState_LBLBBSB == LBLBBSB_RECEIVE_CR4) 
		{
			esp32ReceiveState_LBLBBSB = LBLBBSB_RECEIVE_LF4;
			*pEsp32RxBufferWrite++ = '\0';
			esp32ReceiveState = RECEIVE_OK;
		}
	}
	else
	{
		if (esp32ReceiveState_LBLBBSB == LBLBBSB_RECEIVE_LF3) *pEsp32RxBufferWrite++ = rx1Buffer[0];
	}
}

void Esp32RxCalback_LBBSBBS(void)
{
	if (rx1Buffer[0] == '\r')
	{
		if (esp32ReceiveState_LBBSBBS == LBBSBBS_RECEIVE_START) esp32ReceiveState_LBBSBBS = LBBSBBS_RECEIVE_CR1;
		else if (esp32ReceiveState_LBBSBBS == LBBSBBS_RECEIVE_LF1) esp32ReceiveState_LBBSBBS = LBBSBBS_RECEIVE_CR2;
		else if (esp32ReceiveState_LBBSBBS == LBBSBBS_RECEIVE_LF2) esp32ReceiveState_LBBSBBS = LBBSBBS_RECEIVE_CR3;
		else if (esp32ReceiveState_LBBSBBS == LBBSBBS_RECEIVE_LF3) esp32ReceiveState_LBBSBBS = LBBSBBS_RECEIVE_CR4;
	}
	else if (rx1Buffer[0] == '\n')
	{
		if (esp32ReceiveState_LBBSBBS == LBBSBBS_RECEIVE_CR1) esp32ReceiveState_LBBSBBS = LBBSBBS_RECEIVE_LF1;
		else if (esp32ReceiveState_LBBSBBS == LBBSBBS_RECEIVE_CR2) esp32ReceiveState_LBBSBBS = LBBSBBS_RECEIVE_LF2;
		else if (esp32ReceiveState_LBBSBBS == LBBSBBS_RECEIVE_CR3) esp32ReceiveState_LBBSBBS = LBBSBBS_RECEIVE_LF3;
		else if (esp32ReceiveState_LBBSBBS == LBBSBBS_RECEIVE_CR4) esp32ReceiveState_LBBSBBS = LBBSBBS_RECEIVE_LF4;
	}
	else if (rx1Buffer[0] == '>') 
	{
		esp32ReceiveState_LBBSBBS = LBBSBBS_RECEIVE_LAB;
		*pEsp32RxBufferWrite++ = rx1Buffer[0];
		*pEsp32RxBufferWrite++ = '\0';
		esp32ReceiveState = RECEIVE_OK;
	}
	else
	{
		if (esp32ReceiveState_LBBSBBS == LBBSBBS_RECEIVE_LF4) *pEsp32RxBufferWrite++ = rx1Buffer[0];
	}
}

void Esp32RxCallback(void)
{
	if (esp32ReceiveType == LBBSB)Esp32RxCalback_LBBSB();
	else if (esp32ReceiveType == SB)Esp32RxCalback_SB();
	else if (esp32ReceiveType == BS)Esp32RxCalback_BS();
	else if (esp32ReceiveType == LBLBLBBSB)Esp32RxCalback_LBLBLBBSB();
	else if (esp32ReceiveType == LBLBBSB)Esp32RxCalback_LBLBBSB();
	else if (esp32ReceiveType == LBBSBBS)Esp32RxCalback_LBBSBBS();
}

void SetEsp32BtReceiveType(enum Esp32ReceiveType type)
{
	esp32ReceiveType = type;
	if (type == LBBSB)esp32ReceiveState_LBBSB = LBBSB_RECEIVE_START;
	else if (type == SB)esp32ReceiveState_SB = SB_RECEIVE_START;
	else if (type == BS)esp32ReceiveState_BS = BS_RECEIVE_START;
	else if (type == LBLBLBBSB)esp32ReceiveState_LBLBLBBSB = LBLBLBBSB_RECEIVE_START;
	else if (type == LBLBBSB)esp32ReceiveState_LBLBBSB = LBLBBSB_RECEIVE_START;
	else if (type == LBBSBBS)esp32ReceiveState_LBBSBBS = LBBSBBS_RECEIVE_START;
}

void Esp32RxStateReset(enum Esp32ReceiveType rxType)
{
	SetEsp32BtReceiveType(rxType);
	esp32ReceiveState = RECEIVE_BUSY;
	pEsp32RxBufferWrite = esp32RxBuffer;
}

void Esp32ATTransmit(enum Esp32ReceiveType rxType, uint8_t *pData, uint16_t size)
{
	Esp32RxStateReset(rxType);
	HAL_UART_Transmit(&huart1, pData, size, ESP32_TIMEOUT_LONGSENT);
	HAL_UART_Transmit(&huart1, txEndString, 2, ESP32_TIMEOUT_LONGSENT);
}

uint8_t mstrcmp(const uint8_t *cs, const uint8_t *ct)
{
	uint8_t c1, c2;

	while (1) {
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return 1;
		if (!c1)
			break;
	}
	return 0;
}

void Esp32Send_BtInit(void)
{
	Esp32ATTransmit(LBBSB, (uint8_t*)"AT+BTINIT=1", 11);
	Esp32BlockingReceive();
	if (mstrcmp((uint8_t*)"OK", pEsp32RxBufferRead)) //返回error
		{
			;
		}
}

void Esp32Send_BTSPPINIT(void)
{
	Esp32ATTransmit(LBBSB, (uint8_t*)"AT+BTSPPINIT=2", 14);
	Esp32BlockingReceive();
	if (mstrcmp((uint8_t*)"OK", pEsp32RxBufferRead)) 
	{
		Esp32Send_BtInit();
	}
}

void Esp32Send_BTNAME(void)
{
	Esp32ATTransmit(LBBSB, (uint8_t*)"AT+BTNAME=\"32\"", 14);
	Esp32BlockingReceive();
	if (mstrcmp((uint8_t*)"OK", pEsp32RxBufferRead)) 
	{
		Esp32Send_BtInit();
	}
}

void Esp32Send_BTSCANMODE(void)
{
	Esp32ATTransmit(LBBSB, (uint8_t*)"AT+BTSCANMODE=2", 15);
	Esp32BlockingReceive();
	if (mstrcmp((uint8_t*)"OK", pEsp32RxBufferRead)) 
	{
		Esp32Send_BtInit();
	}
}

void Esp32Send_BTSECPARAM(void)
{
	Esp32ATTransmit(LBBSB, (uint8_t*)"AT+BTSECPARAM=3,1,\"6031\"", 24);
	Esp32BlockingReceive();
	if (mstrcmp((uint8_t*)"OK", pEsp32RxBufferRead)) 
	{
		Esp32Send_BtInit();
	}
}

void Esp32Send_BTSPPSTART(void)
{
	Esp32ATTransmit(LBBSB, (uint8_t*)"AT+BTSPPSTART", 13);
	Esp32BlockingReceive();
	if (mstrcmp((uint8_t*)"OK", pEsp32RxBufferRead)) 
	{
		Esp32Send_BtInit();
	}
}

void Esp32Send_BTSPPSEND(void)
{
	Esp32ATTransmit(LBBSB, (uint8_t*)"AT+BTSPPSEND", 12);
	Esp32BlockingReceive();
	if (mstrcmp((uint8_t*)"OK", pEsp32RxBufferRead)) 
	{
		Esp32Send_BtInit();
	}
}

void Esp32Send_CWMODE(void)
{
	Esp32ATTransmit(LBBSB, (uint8_t*)"AT+CWMODE=1", 11);
	Esp32BlockingReceive();
} 

void Esp32Send_CWJAP(void)//todo
{
	Esp32ATTransmit(LBLBLBBSB, (uint8_t*)"AT+CWJAP=\"360a\",\"0000....\"", 26);
	Esp32BlockingReceive();
}

void Esp32Send_CIPSTART(void)//todo
{
	Esp32ATTransmit(LBLBBSB, (uint8_t*)"AT+CIPSTART=\"TCP\",\"47.97.214.142\",9999", 38);
	Esp32BlockingReceive();
}

void Esp32Send_CIPMODE(void)
{
	Esp32ATTransmit(LBBSB, (uint8_t*)"AT+CIPMODE=1", 12);
	Esp32BlockingReceive();
}

void Esp32Send_CIPSEND(void)//todo
{
	Esp32ATTransmit(LBBSBBS, (uint8_t*)"AT+CIPSEND", 10);
	Esp32BlockingReceive();
}

void Esp32BtInit(void)
{
	Esp32Send_BtInit();
	Esp32Send_BTSPPINIT();
	Esp32Send_BTNAME();
	Esp32Send_BTSCANMODE();
	Esp32Send_BTSECPARAM();
	Esp32Send_BTSPPSTART();
}

void Esp32WifiInit(void)
{
	Esp32Send_CWMODE();
	Esp32Send_CWJAP();
	Esp32Send_CIPSTART();
	Esp32Send_CIPMODE();
	Esp32Send_CIPSEND();
}


void Esp32Init(void)
{
	HAL_UART_Receive_DMA(&huart1, rx1Buffer, 1);
	if (esp32WirelessUse == USE_BLUETOOTH) Esp32BtInit();
	else if (esp32WirelessUse == USE_WIFI) Esp32WifiInit();
}

void Esp32BtWaitingConnect(void)
{
	Esp32RxStateReset(SB);
	Esp32BlockingReceive();
}

void Esp32BtSppSendMode(void)
{
	Esp32Send_BTSPPSEND();	
}

void Esp32Send(uint8_t *pData, uint16_t size)
{
	HAL_UART_Transmit(&huart1, pData, size, ESP32_TIMEOUT_LONGSENT);
}

void Esp32End(void)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)"+++", 3, ESP32_TIMEOUT_LONGSENT);
}


