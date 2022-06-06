#include "heartrate.h"

#define HEARTRATE_TICK_DEVIATION 1
#define MINIMUM_HR_TICK_INTERVAL (20-HEARTRATE_TICK_DEVIATION)
#define MAXIMUM_HR_TICK_INTERVAL (200+HEARTRATE_TICK_DEVIATION)
#define MAXIMUM_HR_THRESHOLD 50
#define MINIMUM_HR_THRESHOLD -100

static int16_t HR_positiveThreshold = MAXIMUM_HR_THRESHOLD;
static int16_t HR_negativeThreshold = MINIMUM_HR_THRESHOLD;
static uint8_t HR_passPositiveThresholdFlag;
static uint8_t halfHeartrateCycleFlag;
static int16_t uHeartrate;
static uint32_t HR_tick;
static uint32_t HR_passPosThresholdTick;
static uint16_t max_Heartrate;
static uint16_t min_Heartrate;
static uint8_t isTureHeartrate;


void SetHeartrateThreshold()
{
	HR_positiveThreshold = max_Heartrate * 0.2 + HR_positiveThreshold * 0.8;
	HR_negativeThreshold = min_Heartrate * 0.2 + HR_negativeThreshold * 0.8;
	if (HR_positiveThreshold < MAXIMUM_HR_THRESHOLD)
	{
		HR_positiveThreshold = MAXIMUM_HR_THRESHOLD;
	}
	if (HR_negativeThreshold > MINIMUM_HR_THRESHOLD)
	{
		HR_negativeThreshold = MINIMUM_HR_THRESHOLD;
	}
}

void SetHeartrate(void)
{
	static uint32_t deltaTick;
	deltaTick = HR_tick - HR_passPosThresholdTick;
	if (deltaTick<MINIMUM_HR_TICK_INTERVAL || deltaTick >MAXIMUM_HR_TICK_INTERVAL) 
	{
		isTureHeartrate = 0;
		return;
	}
	isTureHeartrate = 1;
}

uint8_t HeartrateCount(float* acc)
{
	uHeartrate = *acc;
	++HR_tick;
	if (HR_passPositiveThresholdFlag && uHeartrate < HR_negativeThreshold) 
	{
		HR_passPositiveThresholdFlag = 0;
		SetHeartrate();
		return 1;
	}
	else if (!HR_passPositiveThresholdFlag && uHeartrate > HR_positiveThreshold) 
	{
		HR_passPositiveThresholdFlag = 1;
		HR_passPosThresholdTick = HR_tick;
		
	}
	return 0;
}

uint16_t CalcHeartrate(float *acc)
{
	static uint8_t countHt = 0;
	static uint32_t htStartTick;
	static uint32_t htDeltaTick;
	if (HeartrateCount(acc))
	{
		if (!countHt) htStartTick = HR_tick;
		if (countHt == 4)
		{
			htDeltaTick = HR_tick - htStartTick;
			countHt = 1;
			htStartTick = HR_tick;
			return 60 * 4 * 100 / htDeltaTick;
		}
		
		++countHt;
	}
	return 0;
}

