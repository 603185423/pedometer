#include "bt.h"
#include "esp32.h"

uint8_t BtDataBuffer[BT_DATA_BUFFER_SIZE];

void BtSend(uint8_t size)
{
	Esp32BtSend(BtDataBuffer, size);
}

void BtCheckSum(uint8_t size)
{
	int i;
	BtDataBuffer[0] = BT_DATA_HEADER, BtDataBuffer[size - 1] = BT_DATA_TAIL, BtDataBuffer[size - 2] = BtDataBuffer[1];
	for (i = 0; i < size - 4; i++)BtDataBuffer[size - 2] += BtDataBuffer[i + 2];
}

void BtSendDatapack_3AxisAcc(BtDataPack_3AxisAcc *pdata_3Acc)
{
	*(BtDataBuffer + BT_SIZE_HEADER) = *((uint8_t*)pdata_3Acc + 1);
	*(BtDataBuffer + BT_SIZE_HEADER + 1) = *((uint8_t*)pdata_3Acc);
	*(BtDataBuffer + BT_SIZE_HEADER + 2) = *((uint8_t*)pdata_3Acc + 3);
	*(BtDataBuffer + BT_SIZE_HEADER + 3) = *((uint8_t*)pdata_3Acc + 2);
	*(BtDataBuffer + BT_SIZE_HEADER + 4) = *((uint8_t*)pdata_3Acc + 5);
	*(BtDataBuffer + BT_SIZE_HEADER + 5) = *((uint8_t*)pdata_3Acc + 4);
	BtCheckSum(BT_DATA_SIZE_3AXISACC);
	BtSend(BT_DATA_SIZE_3AXISACC);
}

void BtInit(void)
{
	Esp32Init();
	Esp32BtWaitingConnect();
	Esp32BtSppSendMode();
}

