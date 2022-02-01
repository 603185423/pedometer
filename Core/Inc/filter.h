#ifndef __FILTER_H__
#define __FILTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "arm_math.h"

void Filter(float32_t * pSrc, float32_t * pDst, uint32_t blockSize);
void FilterInit(float32_t);
	
	
#ifdef __cplusplus
}
#endif

#endif /* __FILTER_H__ */
