#include "oled12864.h"
#include "i2c.h"
#include "oledfont.h"

#define OLED_STEP_CHAR_DISPLAY_POSITON_X 0
#define OLED_STEP_CHAR_DISPLAY_POSITON_Y 0

#define OLED_HEARTRATE_CHAR_DISPLAY_POSITON_X 0
#define OLED_HEARTRATE_CHAR_DISPLAY_POSITON_Y 1

#define OLED_STEP_NUM_DISPLAY_POSITON_X 40
#define OLED_STEP_NUM_DISPLAY_POSITON_Y OLED_STEP_CHAR_DISPLAY_POSITON_Y


#define OLED_HEARTRATE_NUM_DISPLAY_POSITON_X 40
#define OLED_HEARTRATE_NUM_DISPLAY_POSITON_Y OLED_HEARTRATE_CHAR_DISPLAY_POSITON_Y




static uint8_t CMD_Data[] =  {
	0xAE,	
	0x00,	
	0x10,	
	0x40,	
	0xB0,
	0x81,
	0xFF,
	0xA1,
	0xA6,
	0xA8,
	0x3F,					
	0xC8,
	0xD3,
	0x00,
	0xD5,
	0x80,
	0xD8,
	0x05,
	0xD9,
	0xF1,
	0xDA,
	0x12,
	0xD8,
	0x30,
	0x8D,
	0x14,	
	0xAF	
};

//向设备写控制命令
void OLED_WR_CMD(uint8_t cmd)
{
	HAL_I2C_Mem_Write(&hi2c1, OLED_DEV_ADDRESS_WR, OLED_MEM_ADDRESS_CMD, I2C_MEMADD_SIZE_8BIT, &cmd, 1, OLED_TIMEOUT_WR);
}
//向设备写数据
void OLED_WR_DATA(uint8_t data)
{
	HAL_I2C_Mem_Write(&hi2c1, OLED_DEV_ADDRESS_WR, OLED_MEM_ADDRESS_DATA, I2C_MEMADD_SIZE_8BIT, &data, 1, OLED_TIMEOUT_WR);
}

//清屏
void OLED_Clear(void)
{
	uint8_t i, n;		    
	for (i = 0; i < 8; i++)  
	{  
		OLED_WR_CMD(0xb0 + i);  //设置页地址（0~7）
		OLED_WR_CMD(0x00);   //设置显示位置—列低地址
		OLED_WR_CMD(0x10);   //设置显示位置—列高地址
		for(n = 0 ; n < 128 ; n++)
			OLED_WR_DATA(0);  //更新显示
	} 
}

//初始化oled屏幕
void OLED_Init(void)
{ 	
	HAL_Delay(200);
 
	uint8_t i = 0;
	for (i = 0; i < 27; i++)
	{
		OLED_WR_CMD(CMD_Data[i]);
	}
	OLED_Clear();
}

//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_CMD(0X8D);   //SET DCDC命令
	OLED_WR_CMD(0X14);   //DCDC ON
	OLED_WR_CMD(0XAF);   //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_CMD(0X8D);   //SET DCDC命令
	OLED_WR_CMD(0X10);   //DCDC OFF
	OLED_WR_CMD(0XAE);   //DISPLAY OFF
}	
//坐标设置 
void OLED_Set_Pos(uint8_t x, uint8_t y) 
{ 	
	OLED_WR_CMD(0xb0 + y);
	OLED_WR_CMD(((x & 0xf0) >> 4) | 0x10);
	OLED_WR_CMD(x & 0x0f);
} 
 
void OLED_On(void)  
{  
	uint8_t i, n;		    
	for (i = 0; i < 8; i++)  
	{  
		OLED_WR_CMD(0xb0 + i);     //设置页地址（0~7）
		OLED_WR_CMD(0x00);       //设置显示位置—列低地址
		OLED_WR_CMD(0x10);       //设置显示位置—列高地址   
		for(n = 0 ; n < 128 ; n++)
			OLED_WR_DATA(1); 
	} //更新显示
}
unsigned int oled_pow(uint8_t m, uint8_t n)
{
	unsigned int result = 1;	 
	while (n--)result *= m;    
	return result;
}
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x, uint8_t y, int num, uint8_t len, OledCharSize size2)
{         	
	uint8_t t, temp;
	uint8_t enshow = 0, flag = 0;	
	if (num < 0)
	{
		num = -num;
		flag = 1;
	}
	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0&&t < (len - 1))
		{
			if (t == 0&&flag)
			{
				OLED_ShowChar(0, y, '-', size2);
				continue;
			}
			if (temp == 0)
			{
				OLED_ShowChar(x + (size2 / 2)*t, y, ' ', size2);
				continue;
			}
			else enshow = 1; 
		 	 
		}
		OLED_ShowChar(x + (size2 / 2)*t, y, temp + '0', size2); 
	}
	if (flag) OLED_ShowChar(0, y, '-', size2);
} 
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, OledCharSize Char_Size)
{      	
	unsigned char c = 0, i = 0;	
	c = chr - ' '; //得到偏移后的值			
	if (x > 128 - 1)
	{
		x = 0;
		y = y + 1;
	}
	if (Char_Size == 16)
	{
		OLED_Set_Pos(x, y);	
		for (i = 0; i < 8; i++)
			OLED_WR_DATA(F8X16[c * 16 + i]);
		OLED_Set_Pos(x, y + 1);
		for (i = 0; i < 8; i++)
			OLED_WR_DATA(F8X16[c * 16 + i + 8]);
	}
	else {	
		OLED_Set_Pos(x, y);
		for (i = 0; i < 6; i++)
			OLED_WR_DATA(F6x8[c][i]);
				
	}
}
 
//显示一个字符号串
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, OledCharSize Char_Size)
{
	unsigned char j = 0;
	while (chr[j] != '\0')
	{		
		OLED_ShowChar(x, y, chr[j], Char_Size); //一次显示一个字符
		x += 7;
		if (x > 120){x = 0; y += 1; }
		j++;
	}
}
//显示汉字
//hzk 用取模软件得出的数组
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no)
{      			    
	uint8_t t, adder = 0;
	//定位（0xbx,x为第几行。0x1x ,x为列地址 高四位；0x0x，x为列地址第4位。
	//比如你想定 位到第3行，第126(0x7e)列，就要写WrCmd(0xb3);WrCmd(0x17);WrCmd(0x0e);）	
		OLED_Set_Pos(x, y);	
	for (t = 0; t < 16; t++)
	{
		OLED_WR_DATA(Hzk[2*no][t]);
		adder += 1;
	}	
	OLED_Set_Pos(x, y + 1);	
	for (t = 0; t < 16; t++)
	{	
		OLED_WR_DATA(Hzk[2*no + 1][t]);
		adder += 1;
	}					
}

void OledDisplayLine(uint8_t *str)
{
	static uint8_t lineNum = 0;
	if (lineNum == OLED_MAX_LINE_8SIZE) lineNum = 0;
	OLED_ShowString(0, lineNum, str, OLED_CHAR_SIZE8);
	++lineNum;
}

void OledStepCountInit(void)
{
	OLED_ShowString(OLED_STEP_CHAR_DISPLAY_POSITON_X, OLED_STEP_CHAR_DISPLAY_POSITON_Y, (uint8_t*)"STEP", OLED_CHAR_SIZE8);
}

void OledHeartrateInit(void)
{
	OLED_ShowString(OLED_HEARTRATE_CHAR_DISPLAY_POSITON_X, OLED_HEARTRATE_CHAR_DISPLAY_POSITON_Y, (uint8_t*)"HEART", OLED_CHAR_SIZE8);
}

void OledShowStepCountNum(uint32_t stepNum)
{
	static uint32_t lastStepNum = 0;
	if (stepNum == lastStepNum)return;
	OLED_ShowNum(OLED_STEP_NUM_DISPLAY_POSITON_X, OLED_STEP_NUM_DISPLAY_POSITON_Y, stepNum, 6, OLED_CHAR_SIZE8);
}

void OledShowHeartrateCountNum(uint32_t heartrateNum)
{
	static uint32_t lastHeartrate = 0;
	if (!heartrateNum)return;
	if (heartrateNum == lastHeartrate)return;		
	OLED_ShowNum(OLED_HEARTRATE_NUM_DISPLAY_POSITON_X, OLED_HEARTRATE_NUM_DISPLAY_POSITON_Y, heartrateNum, 6, OLED_CHAR_SIZE8);
}

void DrawOneHeartrateWave(uint8_t pos, uint8_t num)
{
	static uint8_t line = 0;
	static uint8_t i = 0;
	line = 7 - (num >> 3);
	num &= 0b0111;
	num = 0b10000000 >> num;
	for (i = 4; i < 8; i++)
	{
		OLED_Set_Pos(pos, i);
		if (line == i)
		{
			OLED_WR_DATA(num);
		}
		else
		{
			OLED_WR_DATA(0x00);
		}
	}
}

void DrawHeartrateEndLine(uint8_t pos)
{
	OLED_Set_Pos(pos, 4);
	OLED_WR_DATA(0xFF);
	OLED_Set_Pos(pos, 5);
	OLED_WR_DATA(0xFF);
	OLED_Set_Pos(pos, 6);
	OLED_WR_DATA(0xFF);
	OLED_Set_Pos(pos, 7);
	OLED_WR_DATA(0xFF);
}

void OledDisplayHeartrateWave(uint8_t heartrate)
{
	static uint8_t pos = 0;
	if (pos == 128)pos = 0;
	if (heartrate>159)heartrate=159;
	else if (heartrate<32)heartrate=32;
	heartrate -= 32;
	heartrate = heartrate >> 2;
	DrawOneHeartrateWave(pos, heartrate);
	DrawHeartrateEndLine(pos + 1);
	++pos;
}
