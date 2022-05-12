#ifndef __OLED12832_H__
#define __OLED12832_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
	
#define OLED_MAX_LINE_8SIZE 4
	
#define OLED_STEP_CHAR_DISPLAY_POSITON_X 0
#define OLED_STEP_CHAR_DISPLAY_POSITON_Y 0
#define OLED_STEP_NUM_DISPLAY_POSITON_X 40
#define OLED_STEP_NUM_DISPLAY_POSITON_Y 0
	
	typedef enum OledCharSize{ OLED_CHAR_SIZE8 = 8, OLED_CHAR_SIZE12 = 12, OLED_CHAR_SIZE16 = 16, OLED_CHAR_SIZE24 = 24 }OledCharSize;



	void OLED_ClearPoint(uint8_t x,uint8_t y);
	void OLED_ColorTurn(uint8_t i);
	void OLED_DisplayTurn(uint8_t i);
	void OLED_DisPlay_On(void);
	void OLED_DisPlay_Off(void);
	void OLED_Refresh(void);
	void OLED_Clear(void);
	void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
	void OLED_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode);
	void OLED_DrawCircle(uint8_t x,uint8_t y,uint8_t r);
	void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode);
	void OLED_ShowChar6x8(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode);
	void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size1,uint8_t mode);
	void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode);
	void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t num,uint8_t size1,uint8_t mode);
	void OLED_ScrollDisplay(uint8_t num,uint8_t space,uint8_t mode);
	void OLED_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode);
	void OLED_Init(void);
	void OledDisplayLine(uint8_t *str);
	void OledStepCountInit(void);

#ifdef __cplusplus
}
#endif

#endif /* __OLED12832_H__ */
