#include "filter_hr.h"



#define USE_DF1_F32_2SEC

#ifdef USE_DF1_F32_2SEC
/******************************************
 * 离散时间 IIR 滤波器(实数)
 * 滤波器结构  : 直接 II 型，二阶节
 * 节数     : 2 
 * 稳定     : 是 
 * 线性相位   : 否
 * 
 * SOS 矩阵:
 * 1  0  -1  1  -1.957389039542623443423963180975988507271  0.958531685182608117479219345113961026073
 * 1  0  -1  1  -1.628850768574283680578673738637007772923  0.699463477647529030534201410773675888777
 * 
 * 定标值:
 * 0.128770849454770242026313553651561960578 
 * 0.128770849454770242026313553651561960578  
 *****************************************/
#define IIR_SECTION 2
#define FILTER_STABILITY_TIME 80

void HR_arm_biquad_cascade_df1_init_f32(arm_biquad_casd_df1_inst_f32 * S, uint8_t numStages, float32_t * pCoeffs, float32_t * pState);
void HR_arm_biquad_cascade_df1_f32(const arm_biquad_casd_df1_inst_f32 * S, float32_t * pSrc, float32_t * pDst, uint32_t blockSize);

static float32_t HR_iir_state[4*IIR_SECTION];
static float32_t HR_iir_coeffs[5*IIR_SECTION] = {
	0.128770849454770242026313553651561960578, 0, -0.128770849454770242026313553651561960578, 1.957389039542623443423963180975988507271, -0.958531685182608117479219345113961026073,
	0.128770849454770242026313553651561960578, 0, -0.128770849454770242026313553651561960578, 1.628850768574283680578673738637007772923, -0.699463477647529030534201410773675888777
};
static arm_biquad_casd_df1_inst_f32 HR_S;
typedef void(*HR_pBiquadInit)(arm_biquad_casd_df1_inst_f32*, uint8_t, float32_t*, float32_t*);
HR_pBiquadInit HR_ArmBiquadInit = HR_arm_biquad_cascade_df1_init_f32;

typedef void(*HR_pFilter)(const arm_biquad_casd_df1_inst_f32*, float32_t*, float32_t*, uint32_t);
HR_pFilter HR_ArmFilter = HR_arm_biquad_cascade_df1_f32;

#endif // USE_DF1_F32_2SEC

static uint16_t HR_i;


void HR_FilterInit(float32_t acc)
{
	HR_ArmBiquadInit(&HR_S, IIR_SECTION, HR_iir_coeffs, HR_iir_state);
	float32_t src[FILTER_STABILITY_TIME], dst[FILTER_STABILITY_TIME] = { 0 };
	for (HR_i = 0; HR_i < FILTER_STABILITY_TIME; HR_i++)
	{
		src[HR_i] = acc;
	}
	HR_ArmFilter(&HR_S, src, dst, FILTER_STABILITY_TIME);
	HR_ArmFilter(&HR_S, src, dst, FILTER_STABILITY_TIME);
	HR_ArmFilter(&HR_S, src, dst, FILTER_STABILITY_TIME);
	HR_ArmFilter(&HR_S, src, dst, FILTER_STABILITY_TIME);
}

void HR_filter(float32_t * pSrc, float32_t * pDst, uint32_t blockSize)
{
	HR_ArmFilter(&HR_S, pSrc, pDst,blockSize);
}



void HR_arm_biquad_cascade_df1_init_f32(arm_biquad_casd_df1_inst_f32 * S, uint8_t numStages, float32_t * pCoeffs, float32_t * pState)
{
	S->numStages = numStages;
	S->pCoeffs = pCoeffs;
	memset(pState, 0, (4U * (uint32_t) numStages) * sizeof(float32_t));
	S->pState = pState;
}

void HR_arm_biquad_cascade_df1_f32(const arm_biquad_casd_df1_inst_f32 * S, float32_t * pSrc, float32_t * pDst, uint32_t blockSize)
{
	float32_t *pIn = pSrc;
	float32_t *pOut = pDst;
	float32_t *pState = S->pState;
	float32_t *pCoeffs = S->pCoeffs;
	float32_t acc;
	float32_t b0, b1, b2, a1, a2;
	float32_t Xn1, Xn2, Yn1, Yn2;
	float32_t Xn;
	uint32_t sample, stage = S->numStages;


#if defined (ARM_MATH_DSP)

	do
	{
		b0 = *pCoeffs++;
		b1 = *pCoeffs++;
		b2 = *pCoeffs++;
		a1 = *pCoeffs++;
		a2 = *pCoeffs++;
		Xn1 = pState[0];
		Xn2 = pState[1];
		Yn1 = pState[2];
		Yn2 = pState[3];
		sample = blockSize >> 2U;
		while (sample > 0U)
		{
			Xn = *pIn++;
			Yn2 = (b0 * Xn) + (b1 * Xn1) + (b2 * Xn2) + (a1 * Yn1) + (a2 * Yn2);
			*pOut++ = Yn2;
			Xn2 = *pIn++;
			Yn1 = (b0 * Xn2) + (b1 * Xn) + (b2 * Xn1) + (a1 * Yn2) + (a2 * Yn1);
			*pOut++ = Yn1;
			Xn1 = *pIn++;
			Yn2 = (b0 * Xn1) + (b1 * Xn2) + (b2 * Xn) + (a1 * Yn1) + (a2 * Yn2);
			*pOut++ = Yn2;
			Xn = *pIn++;
			Yn1 = (b0 * Xn) + (b1 * Xn1) + (b2 * Xn2) + (a1 * Yn2) + (a2 * Yn1);
			*pOut++ = Yn1;
			Xn2 = Xn1;
			Xn1 = Xn;
			sample--;
		}
		sample = blockSize & 0x3U;

		while (sample > 0U)
		{
			Xn = *pIn++;
			acc = (b0 * Xn) + (b1 * Xn1) + (b2 * Xn2) + (a1 * Yn1) + (a2 * Yn2);
			*pOut++ = acc;
			Xn2 = Xn1;
			Xn1 = Xn;
			Yn2 = Yn1;
			Yn1 = acc;
			sample--;

		}
		*pState++ = Xn1;
		*pState++ = Xn2;
		*pState++ = Yn1;
		*pState++ = Yn2;
		pIn = pDst;
		pOut = pDst;
		stage--;
	} while (stage > 0U);

#else
	do
	{
		b0 = *pCoeffs++;
		b1 = *pCoeffs++;
		b2 = *pCoeffs++;
		a1 = *pCoeffs++;
		a2 = *pCoeffs++;
		Xn1 = pState[0];
		Xn2 = pState[1];
		Yn1 = pState[2];
		Yn2 = pState[3];
		sample = blockSize;
		while (sample > 0U)
		{
			Xn = *pIn++;
			acc = (b0 * Xn) + (b1 * Xn1) + (b2 * Xn2) + (a1 * Yn1) + (a2 * Yn2);
			*pOut++ = acc;
			Xn2 = Xn1;
			Xn1 = Xn;
			Yn2 = Yn1;
			Yn1 = acc;
			sample--;
		}
		*pState++ = Xn1;
		*pState++ = Xn2;
		*pState++ = Yn1;
		*pState++ = Yn2;
		pIn = pDst;
		pOut = pDst;
		stage--;
	} while (stage > 0U);

#endif /* #if defined (ARM_MATH_DSP) */

}
