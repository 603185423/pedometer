#include "step.h"

#define STAP_TICK_DEVIATION 1
#define MINIMUM_STEP_TICK_INTERVAL (20-STAP_TICK_DEVIATION)
#define MAXIMUM_STEP_TICK_INTERVAL (200+STAP_TICK_DEVIATION)
#define MAXIMUM_STEP_THRESHOLD 20
#define MINIMUM_STEP_THRESHOLD -20

static int16_t positiveThreshold = MAXIMUM_STEP_THRESHOLD;
static int16_t negativeThreshold = MINIMUM_STEP_THRESHOLD;
static uint8_t passPositiveThresholdFlag;
static uint8_t halfStepCycleFlag;
static int16_t uAcc;
static uint32_t tick;
static uint32_t passPosThresholdTick;
static uint16_t max_Acc;
static uint16_t min_Acc;
static uint8_t isTureStep;


void SetThreshold()
{
	positiveThreshold = max_Acc * 0.2 + positiveThreshold * 0.8;
	negativeThreshold = min_Acc * 0.2 + negativeThreshold * 0.8;
	if (positiveThreshold < MAXIMUM_STEP_THRESHOLD)
	{
		positiveThreshold = MAXIMUM_STEP_THRESHOLD;
	}
	if (negativeThreshold > MINIMUM_STEP_THRESHOLD)
	{
		negativeThreshold = MINIMUM_STEP_THRESHOLD;
	}
}

void SetStep(void)
{
	static uint32_t deltaTick;
	deltaTick = tick - passPosThresholdTick;
	if (deltaTick<MINIMUM_STEP_TICK_INTERVAL || deltaTick >MAXIMUM_STEP_TICK_INTERVAL) 
	{
		isTureStep = 0;
		return;
	}
	isTureStep = 1;
}

uint8_t StepCount(float* acc)
{
	uAcc = *acc;
	++tick;
	if (uAcc>max_Acc)max_Acc = uAcc;
	if (uAcc<min_Acc)min_Acc = uAcc;
	if (passPositiveThresholdFlag && uAcc < negativeThreshold) 
	{
		passPositiveThresholdFlag = 0;
		SetStep();
		//SetThreshold();
		max_Acc = 0;
		min_Acc = 0;
		return isTureStep;
	}
	else if (!passPositiveThresholdFlag && uAcc > positiveThreshold) 
	{
		passPositiveThresholdFlag = 1;
		passPosThresholdTick = tick;
		
	}
	return 0;
}

