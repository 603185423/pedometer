#include "bt.h"
#include "esp32.h"

static uint8_t BtDataBuffer[BT_DATA_BUFFER_SIZE];
static uint8_t bt_i;

void BtSend(uint16_t size)
{
	Esp32Send(BtDataBuffer, size);
}

void BtCheckSum(uint8_t size)
{
	BtDataBuffer[0] = BT_DATA_HEADER, BtDataBuffer[size - 1] = BT_DATA_TAIL, BtDataBuffer[size - 2] = BtDataBuffer[1];
	for (bt_i = 0; bt_i < size - 4; bt_i++)BtDataBuffer[size - 2] += BtDataBuffer[bt_i + 2];
}

void BtSendDatapack_3AxisAcc(BtDataPack_3AxisAcc *pdata_3Acc)
{
	*(BtDataBuffer + BT_SIZE_HEADER) = *((uint8_t*)pdata_3Acc);
	*(BtDataBuffer + BT_SIZE_HEADER + 1) = *((uint8_t*)pdata_3Acc + 1);
	*(BtDataBuffer + BT_SIZE_HEADER + 2) = *((uint8_t*)pdata_3Acc + 2);
	*(BtDataBuffer + BT_SIZE_HEADER + 3) = *((uint8_t*)pdata_3Acc + 3);
	*(BtDataBuffer + BT_SIZE_HEADER + 4) = *((uint8_t*)pdata_3Acc + 4);
	*(BtDataBuffer + BT_SIZE_HEADER + 5) = *((uint8_t*)pdata_3Acc + 5);
	BtCheckSum(BT_DATA_SIZE_3AXISACC);
	BtSend(BT_DATA_SIZE_3AXISACC);
}

void BtSendDatapack_3AxisAccWithTotal(BtDataPack_3AxisAccWithTotal *pdata_3Acc)
{
	*(BtDataBuffer + BT_SIZE_HEADER) = *((uint8_t*)pdata_3Acc);
	*(BtDataBuffer + BT_SIZE_HEADER + 1) = *((uint8_t*)pdata_3Acc + 1);
	*(BtDataBuffer + BT_SIZE_HEADER + 2) = *((uint8_t*)pdata_3Acc + 2);
	*(BtDataBuffer + BT_SIZE_HEADER + 3) = *((uint8_t*)pdata_3Acc + 3);
	*(BtDataBuffer + BT_SIZE_HEADER + 4) = *((uint8_t*)pdata_3Acc + 4);
	*(BtDataBuffer + BT_SIZE_HEADER + 5) = *((uint8_t*)pdata_3Acc + 5);
	*(BtDataBuffer + BT_SIZE_HEADER + 6) = *((uint8_t*)pdata_3Acc + 8);
	*(BtDataBuffer + BT_SIZE_HEADER + 7) = *((uint8_t*)pdata_3Acc + 9);
	*(BtDataBuffer + BT_SIZE_HEADER + 8) = *((uint8_t*)pdata_3Acc + 10);
	*(BtDataBuffer + BT_SIZE_HEADER + 9) = *((uint8_t*)pdata_3Acc + 11);
	BtCheckSum(BT_DATA_SIZE_3AXISACCWITHTOTAL);
	BtSend(BT_DATA_SIZE_3AXISACCWITHTOTAL);
}

void BtInit(void)
{
	esp32WirelessUse = USE_BLUETOOTH;
	Esp32Init();
	Esp32BtWaitingConnect();
	Esp32BtSppSendMode();
}

