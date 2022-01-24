#include "adxl345.h"
#include "i2c.h"


static uint8_t Adxl345ReadBuffer[6];

void ADXL345_Write(uint16_t MemAddress, uint8_t data)
{
	HAL_I2C_Mem_Write(&hi2c1, ADXL345_DEV_ADDRESS_WR, MemAddress, I2C_MEMADD_SIZE_8BIT, &data, 1, ADXL345_TIMEOUT_WR);
}
//向设备写数据
void ADXL345_Read(uint16_t MemAddress, uint8_t *data)
{
	HAL_I2C_Mem_Read(&hi2c1, ADXL345_DEV_ADDRESS_RD, MemAddress, I2C_MEMADD_SIZE_8BIT, data, 1, ADXL345_TIMEOUT_RD);
}



/*ADXL345初始化函数：成功返回0，失败返回1*/
uint8_t ADXL345_Init(void) {				  
	uint8_t id, val;
	HAL_I2C_Mem_Read(&hi2c1, ADXL345_DEV_ADDRESS_RD, ADXL345_REG_DEVICE_ID, I2C_MEMADD_SIZE_8BIT, &id, 1, 0xff);	
	if (id == 0XE5) {
		  	//读器件ID，ADXL345的器件ID为0XE5
		val = 0x2B; 		//低电平中断输出，13位全分辨率，输出数据右对齐，16g量程
		HAL_I2C_Mem_Write(&hi2c1, ADXL345_DEV_ADDRESS_WR, ADXL345_REG_DATA_FORMAT, I2C_MEMADD_SIZE_8BIT, &val, 1, 0xFF);
		val = 0b00001010; 		//数据输出速度为100Hz
		HAL_I2C_Mem_Write(&hi2c1, ADXL345_DEV_ADDRESS_WR, ADXL345_REG_BW_RATE, I2C_MEMADD_SIZE_8BIT, &val, 1, 0xFF);
		val = 0x28; 		//链接使能，测量模式
		HAL_I2C_Mem_Write(&hi2c1, ADXL345_DEV_ADDRESS_WR, ADXL345_REG_POWER_CTL, I2C_MEMADD_SIZE_8BIT, &val, 1, 0xFF);
		val = 0x00; 		//不使用中断
		HAL_I2C_Mem_Write(&hi2c1, ADXL345_DEV_ADDRESS_WR, ADXL345_REG_INT_ENABLE, I2C_MEMADD_SIZE_8BIT, &val, 1, 0xFF);
		HAL_I2C_Mem_Write(&hi2c1, ADXL345_DEV_ADDRESS_WR, ADXL345_REG_OFSX, I2C_MEMADD_SIZE_8BIT, &val, 1, 0xFF);
		HAL_I2C_Mem_Write(&hi2c1, ADXL345_DEV_ADDRESS_WR, ADXL345_REG_OFSY, I2C_MEMADD_SIZE_8BIT, &val, 1, 0xFF);
		HAL_I2C_Mem_Write(&hi2c1, ADXL345_DEV_ADDRESS_WR, ADXL345_REG_OFSZ, I2C_MEMADD_SIZE_8BIT, &val, 1, 0xFF);
		return ADXL345_INIT_OK;
	}			
	return ADXL345_INIT_ERR;	   								  
} 
/*读取ADXL345三个轴的数据*/
void ADXL345_RD_XYZ(short *x, short *y, short *z) {						  		   
	HAL_I2C_Mem_Read(&hi2c1, ADXL345_DEV_ADDRESS_RD, ADXL345_REG_DATA_X0, I2C_MEMADD_SIZE_8BIT, &Adxl345ReadBuffer[0], 1, 0xFF);
	HAL_I2C_Mem_Read(&hi2c1, ADXL345_DEV_ADDRESS_RD, ADXL345_REG_DATA_X1, I2C_MEMADD_SIZE_8BIT, &Adxl345ReadBuffer[1], 1, 0xFF);
	HAL_I2C_Mem_Read(&hi2c1, ADXL345_DEV_ADDRESS_RD, ADXL345_REG_DATA_Y0, I2C_MEMADD_SIZE_8BIT, &Adxl345ReadBuffer[2], 1, 0xFF);
	HAL_I2C_Mem_Read(&hi2c1, ADXL345_DEV_ADDRESS_RD, ADXL345_REG_DATA_Y1, I2C_MEMADD_SIZE_8BIT, &Adxl345ReadBuffer[3], 1, 0xFF);
	HAL_I2C_Mem_Read(&hi2c1, ADXL345_DEV_ADDRESS_RD, ADXL345_REG_DATA_Z0, I2C_MEMADD_SIZE_8BIT, &Adxl345ReadBuffer[4], 1, 0xFF);
	HAL_I2C_Mem_Read(&hi2c1, ADXL345_DEV_ADDRESS_RD, ADXL345_REG_DATA_Z1, I2C_MEMADD_SIZE_8BIT, &Adxl345ReadBuffer[5], 1, 0xFF);	
	*x = (short)(((uint16_t)Adxl345ReadBuffer[1] << 8) + Adxl345ReadBuffer[0]);  	//DATA_X1为高位有效字节    
	*y = (short)(((uint16_t)Adxl345ReadBuffer[3] << 8) + Adxl345ReadBuffer[2]);  	//DATA_Y1为高位有效字节  	    
	*z = (short)(((uint16_t)Adxl345ReadBuffer[5] << 8) + Adxl345ReadBuffer[4]);  	//DATA_Z1为高位有效字节  	   
}
