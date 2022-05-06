/* ************************************************************************** */
/** Descriptive File Name
 * 
 *  an interface for saving user settings.
 */
/* ************************************************************************** */

#ifndef _SAVE_SETTING_H    /* Guard against multiple inclusion */
#define _SAVE_SETTING_H

#include <stdbool.h>
#include <stdint.h>

void ISaveSetting_SetTransmitFunc(void(*Function)(uint8_t *, uint16_t));
void ISaveSetting_SetReceiveFunc(uint8_t(*Function)(void));

/* Used if the transmit function is using SPI as communication peripheral */
void ISaveSetting_SetChipSelectPinFunc(void(*Function)(bool));
void ISaveSetting_SetDelayMsFunc(void(*Function)(uint32_t));
void ISaveSetting_Init(void);

void ISaveSetting_WriteData(uint16_t address, uint8_t *data, uint16_t size);
void ISaveSetting_ReadData(uint16_t address, uint8_t *data, uint16_t size);
    

#endif /* _E2P_H_ */

/* *****************************************************************************
 End of File
 */