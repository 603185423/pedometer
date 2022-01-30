#ifndef __WIFI_H__
#define __WIFI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
	
#define WIFI_DATA_PACK_SIZE_3ACC_WITH_TOTAL		10
#define WIFI_DATA_PACK_SIZE_MIXED_ACC		4
#define WIFI_DATA_SEND_FRC		100
#define WIFI_DATA_BUFFER_SIZE	(WIFI_DATA_PACK_SIZE_3ACC_WITH_TOTAL*WIFI_DATA_SEND_FRC+20)
	
#define WIFI_SIZE_HEADER 1
#define WIFI_DATA_HEADER_3ACC_WITH_TOTAL 0xA5
#define WIFI_DATA_TAIL_3ACC_WITH_TOTAL 0x5A
#define WIFI_DATA_HEADER_MIXED_ACC 0xA6
#define WIFI_DATA_TAIL_MIXED_ACC 0x6A
	typedef struct
	{
		short x;
		short y;
		short z;
		float f;
	}WifiDataPack_3AxisAccWithTotal;
	typedef float WifiDataPack_MixedAcc;
	
	void WifiInit(void);
	void WifiSendDatapack_3AxisAccWithTotal(WifiDataPack_3AxisAccWithTotal *pdata_3Acc);
	void WifiSendDataPack_MixedAcc(WifiDataPack_MixedAcc *pdata_3Acc);
	
	
#ifdef __cplusplus
}
#endif

#endif /* __WIFI_H__ */
