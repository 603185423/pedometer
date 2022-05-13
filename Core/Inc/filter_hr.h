#ifndef __FILTER_HR_H__
#define __FILTER_HR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "arm_math.h"

void HR_filter(float32_t * pSrc, float32_t * pDst, uint32_t blockSize);
void HR_FilterInit(float32_t);
	
	
#ifdef __cplusplus
}
#endif

#endif /* __FILTER_HR_H__ */
