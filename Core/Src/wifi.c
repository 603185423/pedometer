#include "wifi.h"
#include "esp32.h"

static uint8_t WifiDataBuffer[WIFI_DATA_BUFFER_SIZE];
static uint8_t wifiSendCount = 0;

void WifiSend(uint16_t size)
{
	Esp32Send(WifiDataBuffer, size);
}

void WifiSendDatapack_3AxisAccWithTotal(WifiDataPack_3AxisAccWithTotal *pdata_3Acc)
{
	*(WifiDataBuffer + WIFI_SIZE_HEADER + wifiSendCount * WIFI_DATA_PACK_SIZE_3ACC_WITH_TOTAL) = *((uint8_t*)pdata_3Acc);
	*(WifiDataBuffer + WIFI_SIZE_HEADER + 1 + wifiSendCount * WIFI_DATA_PACK_SIZE_3ACC_WITH_TOTAL) = *((uint8_t*)pdata_3Acc + 1);
	*(WifiDataBuffer + WIFI_SIZE_HEADER + 2 + wifiSendCount * WIFI_DATA_PACK_SIZE_3ACC_WITH_TOTAL) = *((uint8_t*)pdata_3Acc + 2);
	*(WifiDataBuffer + WIFI_SIZE_HEADER + 3 + wifiSendCount * WIFI_DATA_PACK_SIZE_3ACC_WITH_TOTAL) = *((uint8_t*)pdata_3Acc + 3);
	*(WifiDataBuffer + WIFI_SIZE_HEADER + 4 + wifiSendCount * WIFI_DATA_PACK_SIZE_3ACC_WITH_TOTAL) = *((uint8_t*)pdata_3Acc + 4);
	*(WifiDataBuffer + WIFI_SIZE_HEADER + 5 + wifiSendCount * WIFI_DATA_PACK_SIZE_3ACC_WITH_TOTAL) = *((uint8_t*)pdata_3Acc + 5);
	*(WifiDataBuffer + WIFI_SIZE_HEADER + 6 + wifiSendCount * WIFI_DATA_PACK_SIZE_3ACC_WITH_TOTAL) = *((uint8_t*)pdata_3Acc + 8);
	*(WifiDataBuffer + WIFI_SIZE_HEADER + 7 + wifiSendCount * WIFI_DATA_PACK_SIZE_3ACC_WITH_TOTAL) = *((uint8_t*)pdata_3Acc + 9);
	*(WifiDataBuffer + WIFI_SIZE_HEADER + 8 + wifiSendCount * WIFI_DATA_PACK_SIZE_3ACC_WITH_TOTAL) = *((uint8_t*)pdata_3Acc + 10);
	*(WifiDataBuffer + WIFI_SIZE_HEADER + 9 + wifiSendCount * WIFI_DATA_PACK_SIZE_3ACC_WITH_TOTAL) = *((uint8_t*)pdata_3Acc + 11);
	++wifiSendCount;
	if (wifiSendCount == 100) 
	{
		WifiDataBuffer[0] = WIFI_DATA_HEADER_3ACC_WITH_TOTAL;
		WifiDataBuffer[1001] = WIFI_DATA_TAIL_3ACC_WITH_TOTAL;
		wifiSendCount = 0;
		WifiSend(1002);
	}
}

void WifiSendDataPack_MixedAcc(WifiDataPack_MixedAcc *pdata_3Acc)
{
	*(WifiDataBuffer + WIFI_SIZE_HEADER + wifiSendCount * WIFI_DATA_PACK_SIZE_MIXED_ACC) = *((uint8_t*)pdata_3Acc);
	*(WifiDataBuffer + WIFI_SIZE_HEADER + 1 + wifiSendCount * WIFI_DATA_PACK_SIZE_MIXED_ACC) = *((uint8_t*)pdata_3Acc + 1);
	*(WifiDataBuffer + WIFI_SIZE_HEADER + 2 + wifiSendCount * WIFI_DATA_PACK_SIZE_MIXED_ACC) = *((uint8_t*)pdata_3Acc + 2);
	*(WifiDataBuffer + WIFI_SIZE_HEADER + 3 + wifiSendCount * WIFI_DATA_PACK_SIZE_MIXED_ACC) = *((uint8_t*)pdata_3Acc + 3);

	++wifiSendCount;
	if (wifiSendCount == 40) 
	{
		WifiDataBuffer[0] = WIFI_DATA_HEADER_MIXED_ACC;
		WifiDataBuffer[161] = WIFI_DATA_HEADER_MIXED_ACC;
		wifiSendCount = 0;
		WifiSend(162);
	}
}

void WifiInit(void)
{
	esp32WirelessUse = USE_WIFI;
	Esp32Init();
	WifiDataBuffer[0] = WIFI_DATA_HEADER_3ACC_WITH_TOTAL;
	WifiDataBuffer[1001] = WIFI_DATA_TAIL_3ACC_WITH_TOTAL;
}
