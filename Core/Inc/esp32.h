#ifndef __ESP32_H__
#define __ESP32_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define		ESP32_RX_BUFFER_SIZE	100
#define		ESP32_TIMEOUT			0
#define		ESP32_TIMEOUT_LONGSENT	0x100
	extern uint8_t esp32RxBuffer[ESP32_RX_BUFFER_SIZE];
	extern uint8_t rx1Buffer[1];
	extern enum Esp32WirelessUse{USE_BLUETOOTH, USE_WIFI, NO_WIRELESS}esp32WirelessUse;
	
	
	void Esp32RxCallback(void);
	void Esp32Init(void);
	void Esp32BtWaitingConnect(void);
	void Esp32BtSppSendMode(void);
	void Esp32Send(uint8_t *pData, uint16_t size);
	void Esp32End(void);
	
	
#ifdef __cplusplus
}
#endif

#endif /* __ESP32_H__ */

