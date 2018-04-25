#include "cmsis_os.h"

#include "stm32f4xx_hal.h"
#include "main.h"

#include <stdio.h>
#include <MC.h>
#include <stdlib.h>
#include <string.h>
#include "ff.h"
#include "SDCard.h"

TaskHandle_t SDCard_Task_Handle = NULL;
void Start_SDCard_Task(void);

extern RTC_HandleTypeDef hrtc;

static FATFS FatFs;
static void SDCard_Task(void * pvParameters);

uint32_t fileCount = 0;
FIL Fil;
_Bool first = 1;




SDCardList_t* Log(uint16_t data) {
	SDCardList_t* SDCardList = malloc(sizeof(SDCardList_t));
	if (SDCardList == NULL)
		Error_Handler();
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	HAL_RTC_GetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
	SDCardList->Length = sprintf(SDCardList->Data, "[%04d-%02d-%02d %02d:%02d:%02d]", 2000 + RTC_DateStructure.Year, RTC_DateStructure.Month, RTC_DateStructure.Date, RTC_TimeStructure.Hours, RTC_TimeStructure.Minutes, RTC_TimeStructure.Seconds);

	for (int i = 0; i < 100; i++)
		SDCardList->Length += sprintf(&SDCardList->Data[SDCardList->Length], " ,%d", data + i);
	SDCardList->Length += sprintf(&SDCardList->Data[SDCardList->Length], "\r\n");
	return SDCardList;
}
void Start_SDCard_Task(void) {
	xTaskCreate(SDCard_Task,     				// Function that implements the task.
	"SDCard_Task",   				// Text name for the task.
	configMINIMAL_STACK_SIZE * 10,      	// Stack size in words, not bytes.
	(void *) 0,    				// Parameter passed into the task.
	configMAX_PRIORITIES - 2,				// Priority at which the task is created.
	&SDCard_Task_Handle);      	// Used to pass out the created task's handle.

	if (SDCard_Task_Handle == NULL)
		Error_Handler();

}

void ChangeFile() {
	FRESULT res;
	if (!first) {
		res = f_close(&Fil);
		if (res != FR_OK)
			Error_Handler();
	}

	char str[15] = { 0 };
	sprintf(str, "file_%lu.txt", fileCount++);
	res = f_open(&Fil, str, FA_OPEN_APPEND | FA_WRITE | FA_READ);
	if (res == FR_NO_FILESYSTEM)
		Error_Handler();
	if (res != FR_OK)
		Error_Handler();
}

void SDCard_Task(void * pvParameters) {
	UNUSED(pvParameters);
	osDelay(1000);
	if (f_mount(&FatFs, "", 0) != FR_OK)
		Error_Handler();
	FRESULT res = FR_OK;
	UINT bw;
	ChangeFile();
	first = 0;

	int a = 0;
	for (;;) {

		SDCardList_t* msg = Log(a++);
		//a++;
		osDelay(25);
		res = f_write(&Fil, msg->Data, msg->Length, &bw);
		if (res != FR_OK)
			Error_Handler();
		if (msg->Length != (int) bw)
			Error_Handler();

		free(msg);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);

		if (a % 500 == 0)
			ChangeFile();
		osDelay(25);
	}
}

