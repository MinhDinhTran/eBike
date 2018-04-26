#include "SDCard_Buffer.h"
#include "stm32f4xx_hal.h"

#define ID_SROVE 		1
#define ID_ITAMPA 		2

#define BUFFER_CAN_READ 		0x01
#define BUFFER_READING_DATA 	0x02
#define BUFFER_IS_INIT 			0x04
uint8_t Buffer_State = 0;
cJSON *root = NULL;
cJSON *root_Itampa = NULL;
cJSON *root_Srove = NULL;

int DataLimiter = 0;
extern RTC_HandleTypeDef hrtc;

uint16_t Buffer_CanRead() {
	return Buffer_State & BUFFER_CAN_READ;
}

void Buffer_Init() {
	if (Buffer_State & BUFFER_READING_DATA)
		return;
	if (Buffer_State & BUFFER_IS_INIT)
		Buffer_State |= BUFFER_CAN_READ;
	else {

		if (DataLimiter > 500)
			return;
		char data[25] = { 0 };
		RTC_TimeTypeDef RTC_TimeStructure;
		RTC_DateTypeDef RTC_DateStructure;
		HAL_RTC_GetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
		sprintf(data, "%04d-%02d-%02d %02d:%02d:%02d", 2000 + RTC_DateStructure.Year, RTC_DateStructure.Month, RTC_DateStructure.Date, RTC_TimeStructure.Hours, RTC_TimeStructure.Minutes, RTC_TimeStructure.Seconds);

		root = cJSON_CreateObject();
		cJSON_AddItemToObject(root, "Data", cJSON_CreateString(data));
		cJSON_AddItemToObject(root, "Itampa", root_Itampa = cJSON_CreateArray());
		cJSON_AddItemToObject(root, "Srove", root_Srove = cJSON_CreateArray());

		Buffer_State |= BUFFER_IS_INIT;
	}

}

char* Buffer_GetString() {
	if (!(Buffer_State & BUFFER_CAN_READ)) {
		Buffer_OnReadFinish();
		return 0;
	}

	return cJSON_Print(root);
}
void Buffer_AddValue(uint16_t u_value, uint16_t i_value) {
	if (Buffer_State & BUFFER_READING_DATA)
		return;
	if (!(Buffer_State & BUFFER_IS_INIT))
		return;
	if (root == NULL || root_Itampa == NULL || root_Srove == NULL)
		return;
	if (DataLimiter > 500)
		return;
	cJSON_AddItemToArray(root_Itampa, cJSON_CreateNumber(u_value));
	cJSON_AddItemToArray(root_Srove, cJSON_CreateNumber(i_value));
	DataLimiter++;
}
void Buffer_OnReadFinish() {
	cJSON_Delete(root);
	Buffer_State = 0;
	DataLimiter = 0;
}

