#ifndef __OLED_H__
#define __OLED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


#define OLED_DEV_ADDRESS_WR 0x78
#define OLED_MEM_ADDRESS_CMD 0x00
#define OLED_MEM_ADDRESS_DATA 0x40
#define OLED_TIMEOUT_WR 100
#define OLED_MAX_LINE_8SIZE 8
	
	typedef enum OledCharSize{ OLED_CHAR_SIZE8 = 12, OLED_CHAR_SIZE12 = 16 }OledCharSize;


	void OLED_WR_CMD(uint8_t cmd);
	void OLED_WR_DATA(uint8_t data);
	void OLED_Init(void);
	void OLED_Clear(void);
	void OLED_Display_On(void);
	void OLED_Display_Off(void);
	void OLED_Set_Pos(uint8_t x, uint8_t y);
	void OLED_On(void);
	void OLED_ShowNum(uint8_t x, uint8_t y, int num, uint8_t len, OledCharSize size2);
	void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, OledCharSize Char_Size);
	void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, OledCharSize Char_Size);
	void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);
	void OledDisplayLine(uint8_t *str);


#ifdef __cplusplus
}
#endif

#endif /* __OLED_H__ */
