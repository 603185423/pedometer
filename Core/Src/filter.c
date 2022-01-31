#include "filter.h"
#include "arm_math.h"
#include "main.h"

void arm_biquad_cascade_df1_f32(
  const arm_biquad_casd_df1_inst_f32 * S,
	float32_t * pSrc,
	float32_t * pDst,
	uint32_t blockSize)
{
	float32_t *pIn = pSrc; /*  source pointer            */
	float32_t *pOut = pDst; /*  destination pointer       */
	float32_t *pState = S->pState; /*  pState pointer            */
	float32_t *pCoeffs = S->pCoeffs; /*  coefficient pointer       */
	float32_t acc; /*  Simulates the accumulator */
	float32_t b0, b1, b2, a1, a2; /*  Filter coefficients       */
	float32_t Xn1, Xn2, Yn1, Yn2; /*  Filter pState variables   */
	float32_t Xn; /*  temporary input           */
	uint32_t sample, stage = S->numStages; /*  loop counters             */


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