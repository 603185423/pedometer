#ifndef __HEARTRATE_H__
#define __HEARTRATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
	
	uint8_t HeartrateCount(float* acc);
	uint16_t CalcHeartrate(float *acc);
	
	
	
#ifdef __cplusplus
}
#endif

#endif /* __HEARTRATE_H__ */
