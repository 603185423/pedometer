#include "step.h"

#define STAP_TICK_DEVIATION 1
#define MINIMUM_STEP_TICK_INTERVAL (20-STAP_TICK_DEVIATION)
#define MAXIMUM_STAP_TICK_INTERVAL (200+STAP_TICK_DEVIATION)

static int16_t positiveThreshold = 30;
static int16_t negativeThreshold = -30;
static uint8_t passPositiveThresholdFlag;
static uint8_t halfStepCycleFlag;
static int16_t uAcc;
static uint32_t tick;
static uint32_t lastStepTick;



uint8_t SetStep(void)
{
	static uint32_t deltaTick;
	deltaTick = tick - lastStepTick;
	if (deltaTick<MINIMUM_STEP_TICK_INTERVAL || deltaTick >MAXIMUM_STAP_TICK_INTERVAL) 
	{
		//return 0;
	}
	lastStepTick = tick;
	return 1;
}

uint8_t StepCount(float* acc)
{
	uAcc = *acc;
	++tick;
	if (passPositiveThresholdFlag && uAcc > positiveThreshold) 
	{
		passPositiveThresholdFlag = 0;
	}
	else if (!passPositiveThresholdFlag && uAcc < negativeThreshold) 
	{
		passPositiveThresholdFlag = 1;
		return SetStep();
	}
	return 0;
}

