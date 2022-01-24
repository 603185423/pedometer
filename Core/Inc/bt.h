#ifndef __BT_H__
#define __BT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
	
#define BT_DATA_BUFFER_SIZE 20
	
#define BT_DATA_HEADER 0xA5
#define BT_SIZE_HEADER 1
#define BT_DATA_TAIL 0x5A
#define BT_DATA_SIZE_STRUCT 3
#define BT_DATA_SIZE_3AXISACC (BT_DATA_SIZE_STRUCT + 6)
#define BT_DATA_SIZE_3AXISACCWITHTOTAL (BT_DATA_SIZE_STRUCT + 10)
	
	typedef struct
	{
		short x;
		short y;
		short z;
	}BtDataPack_3AxisAcc;
	typedef struct
	{
		short x;
		short y;
		short z;
		float f;
	}BtDataPack_3AxisAccWithTotal;
	
	void BtSendDatapack_3AxisAcc(BtDataPack_3AxisAcc *pdata_3Acc);
	void BtSendDatapack_3AxisAccWithTotal(BtDataPack_3AxisAccWithTotal *pdata_3Acc);
	void BtInit(void);
	
	
#ifdef __cplusplus
}
#endif

#endif /* __BT_H__ */
