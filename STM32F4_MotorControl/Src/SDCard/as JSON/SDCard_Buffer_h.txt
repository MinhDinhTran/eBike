#ifndef SDCARD_SDCARD_BUFFER_H_
#define SDCARD_SDCARD_BUFFER_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

uint16_t Buffer_CanRead();
void Buffer_Init();
char* Buffer_GetString();
void Buffer_AddValue(uint16_t u_value, uint16_t i_value);
void Buffer_OnReadFinish();



#endif /* SDCARD_SDCARD_BUFFER_H_ */
