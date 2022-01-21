#include "esp32.h"
#include <usart.h>
#define ESP32_BT_DEBUG

enum Esp32BtReceiveState_LBBSB { LBBSB_RECEIVE_START, LBBSB_RECEIVE_CR1, LBBSB_RECEIVE_LF1, LBBSB_RECEIVE_CR2, LBBSB_RECEIVE_LF2, LBBSB_RECEIVE_CR3, LBBSB_RECEIVE_LF3 }esp32BtReceiveState_LBBSB;
enum Esp32BtReceiveState_SB { SB_RECEIVE_START, SB_RECEIVE_CR1, SB_RECEIVE_LF1 }esp32BtReceiveState_SB;
enum Esp32BtReceiveState_BS { BS_RECEIVE_START, BS_RECEIVE_CR1, BS_RECEIVE_LF1, BS_RECEIVE_LAB }esp32BtReceiveState_BS;//LAB:left angle brcket
enum Esp32BtReceiveType { RXNONE, LBBSB, SB, BS }esp32BtReceiveType;
enum Esp32BtReceiveState { RECEIVE_OK, RECEIVE_BUSY }esp32BtReceiveState;
uint8_t rx1Buffer[1], txEndString[] = "\r\n";
uint8_t esp32RxBuffer[ESP32_RX_BUFFER_SIZE];
uint8_t *pEsp32RxBufferWrite = esp32RxBuffer;
uint8_t *pEsp32RxBufferRead = esp32RxBuffer;




uint8_t a = 0;


void Esp32BlockingReceive(void)
{
	while (esp32BtReceiveState == RECEIVE_BUSY)
	{
		;
	}
	esp32BtReceiveType = RXNONE;
#ifdef ESP32_BT_DEBUG
	HAL_UART_Transmit(&huart2, pEsp32RxBufferRead, pEsp32RxBufferWrite - pEsp32RxBufferRead, ESP32_TIMEOUT_LONGSENT);
#endif // ESP32_BT_DEBUG
}

void Esp32BtRxCalback_LBBSB(void)
{
	if (rx1Buffer[0] == '\r')
	{
		if (esp32BtReceiveState_LBBSB == LBBSB_RECEIVE_START) esp32BtReceiveState_LBBSB = LBBSB_RECEIVE_CR1;
		else if (esp32BtReceiveState_LBBSB == LBBSB_RECEIVE_LF1) esp32BtReceiveState_LBBSB = LBBSB_RECEIVE_CR2;
		else if (esp32BtReceiveState_LBBSB == LBBSB_RECEIVE_LF2) esp32BtReceiveState_LBBSB = LBBSB_RECEIVE_CR3;
	}
	else if (rx1Buffer[0] == '\n')
	{
		if (esp32BtReceiveState_LBBSB == LBBSB_RECEIVE_CR1) esp32BtReceiveState_LBBSB = LBBSB_RECEIVE_LF1;
		else if (esp32BtReceiveState_LBBSB == LBBSB_RECEIVE_CR2) esp32BtReceiveState_LBBSB = LBBSB_RECEIVE_LF2;
		else if (esp32BtReceiveState_LBBSB == LBBSB_RECEIVE_CR3) 
		{
			esp32BtReceiveState_LBBSB = LBBSB_RECEIVE_LF3;
			*pEsp32RxBufferWrite++ = '\0';
			esp32BtReceiveState = RECEIVE_OK;
		}
	}
	else
	{
		if (esp32BtReceiveState_LBBSB == LBBSB_RECEIVE_LF2) *pEsp32RxBufferWrite++ = rx1Buffer[0];
	}
}

void Esp32BtRxCalback_SB(void)
{
	if (rx1Buffer[0] == '\r' && esp32BtReceiveState_SB == SB_RECEIVE_START) esp32BtReceiveState_SB = SB_RECEIVE_CR1;
	else if (rx1Buffer[0] == '\n' && esp32BtReceiveState_SB == SB_RECEIVE_CR1)
	{
		esp32BtReceiveState_SB = SB_RECEIVE_LF1;
		*pEsp32RxBufferWrite++ = '\0';
		esp32BtReceiveState = RECEIVE_OK;
	}
	else
	{
		if (esp32BtReceiveState_SB == SB_RECEIVE_START) *pEsp32RxBufferWrite++ = rx1Buffer[0];
	}
}

void Esp32BtRxCalback_BS(void)
{
	if (rx1Buffer[0] == '\r' && esp32BtReceiveState_BS == BS_RECEIVE_START) esp32BtReceiveState_BS = BS_RECEIVE_CR1;
	else if (rx1Buffer[0] == '\n' && esp32BtReceiveState_BS == BS_RECEIVE_CR1) esp32BtReceiveState_BS = BS_RECEIVE_LF1;
	else if (rx1Buffer[0] == '>' && esp32BtReceiveState_BS == BS_RECEIVE_LF1)
	{
		esp32BtReceiveState_BS = BS_RECEIVE_LAB;
		*pEsp32RxBufferWrite++ = rx1Buffer[0];
		*pEsp32RxBufferWrite++ = '\0';
		esp32BtReceiveState = RECEIVE_OK;
	}
}

void Esp32BtRxCalback(void)
{
	if (esp32BtReceiveType == LBBSB)Esp32BtRxCalback_LBBSB();
	else if (esp32BtReceiveType == SB)Esp32BtRxCalback_SB();
	else if (esp32BtReceiveType == BS)Esp32BtRxCalback_BS();
}

void Esp32RxCallback(void)
{
	Esp32BtRxCalback();
}

void SetEsp32BtReceiveType(enum Esp32BtReceiveType type)
{
	esp32BtReceiveType = type;
	if (type == LBBSB)esp32BtReceiveState_LBBSB = LBBSB_RECEIVE_START;
	else if (type == SB)esp32BtReceiveState_SB = SB_RECEIVE_START;
}

void Esp32BtRxStateReset(enum Esp32BtReceiveType rxType)
{
	SetEsp32BtReceiveType(rxType);
	esp32BtReceiveState = RECEIVE_BUSY;
	pEsp32RxBufferWrite = esp32RxBuffer;
}

void Esp32ATTransmit(enum Esp32BtReceiveType rxType, uint8_t *pData, uint16_t size)
{
	Esp32BtRxStateReset(rxType);
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

void Esp32BtInit(void)
{
	Esp32Send_BtInit();
	Esp32Send_BTSPPINIT();
	Esp32Send_BTNAME();
	Esp32Send_BTSCANMODE();
	Esp32Send_BTSECPARAM();
	Esp32Send_BTSPPSTART();
}

void Esp32Init(void)
{
	HAL_UART_Receive_DMA(&huart1, rx1Buffer, 1);
	Esp32BtInit();
}

void Esp32BtWaitingConnect(void)
{
	Esp32BtRxStateReset(SB);
	Esp32BlockingReceive();
}

void Esp32BtSppSendMode(void)
{
	Esp32Send_BTSPPSEND();
//	Esp32BtRxStateReset(BS);
//	Esp32BlockingReceive();
	
}

void Esp32BtSend(uint8_t *pData, uint8_t size)
{
	HAL_UART_Transmit(&huart1, pData, size, ESP32_TIMEOUT_LONGSENT);
}

void Esp32BtEnd(void)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)"+++", 3, ESP32_TIMEOUT_LONGSENT);
}

