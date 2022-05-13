#include  "max30102.h"
#include  "i2c.h"
#include  "filter_hr.h"

static uint16_t heartRate=0;
//uint32_t aun_ir_buffer[500]; //IR LED sensor data
int32_t n_ir_buffer_length=500; //data length
//uint32_t aun_red_buffer[500]; //Red LED sensor data
//int32_t n_sp02; //SPO2 value
//int8_t ch_spo2_valid; //indicator to show if the SP02 calculation is valid
//int32_t n_heart_rate; //heart rate value
//int8_t  ch_hr_valid; //indicator to show if the heart rate calculation is valid



void MAX30102_Write(uint16_t MemAddress, uint8_t data)
{
	HAL_I2C_Mem_Write(&hi2c1, MAX30102_DEV_ADDRESS_WR, MemAddress, I2C_MEMADD_SIZE_8BIT, &data, 1, MAX30102_TIMEOUT_WR);
}

void MAX30102_Read(uint16_t MemAddress, uint8_t *data)
{
	HAL_I2C_Mem_Read(&hi2c1, MAX30102_DEV_ADDRESS_RD, MemAddress, I2C_MEMADD_SIZE_8BIT, data, 1, MAX30102_TIMEOUT_RD);
}

void MAX30102_Writes(uint16_t MemAddress, uint8_t data, uint16_t size)
{
	HAL_I2C_Mem_Write(&hi2c1, MAX30102_DEV_ADDRESS_WR, MemAddress, I2C_MEMADD_SIZE_8BIT, &data, size, MAX30102_TIMEOUT_WR);
}

void MAX30102_Reads(uint16_t MemAddress, uint8_t *data, uint16_t size)
{
	HAL_I2C_Mem_Read(&hi2c1, MAX30102_DEV_ADDRESS_RD, MemAddress, I2C_MEMADD_SIZE_8BIT, data, size, MAX30102_TIMEOUT_RD);
}

/**
* \brief        Read a MAX30102 register
* \par          Details
*               This function reads a MAX30102 register
*
* \param[in]    uch_addr    - register address
* \param[out]   puch_data    - pointer that stores the register data
*
* \retval       true on success
*/
void MAX30102_Clear_Reg(uint8_t uch_addr)
{
	MAX30102_Write(uch_addr, 0);
}


void MAX30102_Reset()
{
	MAX30102_Write(MAX30102_REG_MODE_CONFIG, 0x40);
	HAL_Delay(100);
}

void MAX30102_Init()
{
	MAX30102_Reset();
	uint8_t uch_dummy;
	MAX30102_Read(MAX30102_REG_INTR_STATUS_1, &uch_dummy); //read and clear status register
	MAX30102_Write(MAX30102_REG_INTR_ENABLE_1, 0xc0); // INTR setting
	MAX30102_Write(MAX30102_REG_INTR_ENABLE_2, 0x00);
	MAX30102_Write(MAX30102_REG_FIFO_WR_PTR, 0x00); //FIFO_WR_PTR[4:0]
	MAX30102_Write(MAX30102_REG_OVF_COUNTER, 0x00); //OVF_COUNTER[4:0]
	MAX30102_Write(MAX30102_REG_FIFO_RD_PTR, 0x00); //FIFO_RD_PTR[4:0]
	MAX30102_Write(MAX30102_REG_FIFO_CONFIG, 0x0f); //sample avg = 1, fifo rollover=false, fifo almost full = 17
	MAX30102_Write(MAX30102_REG_MODE_CONFIG, 0x03); //0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
	MAX30102_Write(MAX30102_REG_SPO2_CONFIG, 0x27); // SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)
	MAX30102_Write(MAX30102_REG_LED1_PA, 0x24); //Choose value for ~ 7mA for LED1
	MAX30102_Write(MAX30102_REG_LED2_PA, 0x24); // Choose value for ~ 7mA for LED2
	MAX30102_Write(MAX30102_REG_PILOT_PA, 0x7f); // Choose value for ~ 25mA for Pilot LED
}

void MAX30102_Read_Fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led)
/**
* \brief        Read a set of samples from the MAX30102 FIFO register
* \par          Details
*               This function reads a set of samples from the MAX30102 FIFO register
*
* \param[out]   *pun_red_led   - pointer that stores the red LED reading data
* \param[out]   *pun_ir_led    - pointer that stores the IR LED reading data
*
* \retval       true on success
*/
{
	uint32_t un_temp;
	unsigned char uch_temp;
	*pun_red_led = 0;
	*pun_ir_led = 0;
	uint8_t ach_i2c_data[6];
  
	//read and auto clear status register	
	MAX30102_Read(MAX30102_REG_INTR_STATUS_1, &uch_temp);
	MAX30102_Read(MAX30102_REG_INTR_STATUS_2, &uch_temp);
	
  
	ach_i2c_data[0] = MAX30102_REG_FIFO_DATA;
	MAX30102_Reads(MAX30102_REG_FIFO_DATA, ach_i2c_data, 6);
	un_temp = (unsigned char) ach_i2c_data[0];
	un_temp <<= 16;
	*pun_red_led += un_temp;
	un_temp = (unsigned char) ach_i2c_data[1];
	un_temp <<= 8;
	*pun_red_led += un_temp;
	un_temp = (unsigned char) ach_i2c_data[2];
	*pun_red_led += un_temp;
  
	un_temp = (unsigned char) ach_i2c_data[3];
	un_temp <<= 16;
	*pun_ir_led += un_temp;
	un_temp = (unsigned char) ach_i2c_data[4];
	un_temp <<= 8;
	*pun_ir_led += un_temp;
	un_temp = (unsigned char) ach_i2c_data[5];
	*pun_ir_led += un_temp;
	*pun_red_led &= 0x03FFFF; //Mask MSB [23:18]
	*pun_ir_led &= 0x03FFFF; //Mask MSB [23:18]
  
  
}


//void HeartrateTick()
//{
//	static uint16_t tick = 0;
//	if (tick >= n_ir_buffer_length)
//	{
//		tick = 0;
//		maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 
//		if (!ch_hr_valid) 
//		{
//			tick = 0;
//			return;
//		}
//		heartRate = n_heart_rate;
//	}
//	MAX30102_Read_Fifo((aun_red_buffer + tick), (aun_ir_buffer + tick));
//	tick++;
//}


float GetHeartWave()
{
	uint32_t red_led, ir_led;
	MAX30102_Read_Fifo(&red_led, &ir_led);
	return (float)(red_led >> 2);
}

float GetHeartWaveWithFilter()
{
	static float32_t heartwaveSrc, heartwaveDst;
	heartwaveSrc = GetHeartWave();
	HR_filter(&heartwaveSrc, &heartwaveDst, 1);
	return heartwaveDst;
}

void HeartrateTick()
{
	static uint16_t tick = 0;
	
	if (tick >= n_ir_buffer_length)
	{
		tick = 0;
	}
	
	GetHeartWaveWithFilter();
	
	
	tick++;
}

void HeartRateInit()
{
	MAX30102_Init();
	HR_FilterInit(GetHeartWave());
}

uint16_t GetHeartrate()
{
	HeartrateTick();
	return heartRate;
}




