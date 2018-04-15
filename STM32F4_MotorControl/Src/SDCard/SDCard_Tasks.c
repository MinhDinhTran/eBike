#include "cmsis_os.h"

#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../SDCard/SDCard.h"


TaskHandle_t SDCard_Task_Handle = NULL;
static FATFS FatFs;
static QueueHandle_t xQueue_SDCard;

void SDCard_Task( void * pvParameters );
void Start_SDCard_Task( void );

void WriteMessageToFile(xMessage *msg);

int buttonPressCount = 0;
extern RTC_HandleTypeDef hrtc;
void nOnButtonClick(void)
{
	char data[35];
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
    HAL_RTC_GetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
    sprintf(data,"[%d-%d-%d %d:%d:%d] press no:%i \r\n",\
    		2000 + RTC_DateStructure.Year, RTC_DateStructure.Month, RTC_DateStructure.Date,  \
			RTC_TimeStructure.Hours, RTC_TimeStructure.Minutes, RTC_TimeStructure.Seconds, \
			buttonPressCount++);

	xMessage pxMessage = (xMessage){
	.ucMessageID = 'A'};
	strcpy(pxMessage.ucData, data);

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xQueueSendFromISR( xQueue_SDCard, &pxMessage, &xHigherPriorityTaskWoken );
}

void Start_SDCard_Task( void )
{
	xQueue_SDCard = xQueueCreate( 31, sizeof( xMessage ) );
    xTaskCreate(SDCard_Task,     				// Function that implements the task.
				"SDCard_Task",   				// Text name for the task.
				configMINIMAL_STACK_SIZE,      	// Stack size in words, not bytes.
				( void * ) 1,    				// Parameter passed into the task.
				tskIDLE_PRIORITY,				// Priority at which the task is created.
				&SDCard_Task_Handle );      	// Used to pass out the created task's handle.
}




void SDCard_Task( void * pvParameters )
{
	UNUSED(pvParameters);
	xMessage pxRxedMessage;

	configASSERT( SDCard_Task_Handle != NULL );
	if (f_mount(&FatFs, "", 0) != FR_OK)
		Error_Handler();


	nOnButtonClick();

	for( ;; )
	{
		if( xQueue_SDCard == NULL ) continue;

		if( xQueueReceive( xQueue_SDCard, &( pxRxedMessage ), osWaitForever))//( TickType_t ) 10 ) )
		{
			WriteMessageToFile(&pxRxedMessage);
		}
	}
}

void WriteMessageToFile(xMessage *msg)
{
	FIL Fil;
	FRESULT res = FR_OK;
	UINT bw;
    UINT rxSize = strlen(msg->ucData);
	res = f_open(&Fil, "file.txt", FA_OPEN_APPEND | FA_WRITE | FA_READ);
	if( res != FR_OK) return;
	res = f_write(&Fil, msg->ucData, rxSize, &bw);
	if( res != FR_OK) return;
	res = f_close(&Fil);
	if( res != FR_OK) return;
	if (bw == rxSize)
	{
		//BSP_LED_Toggle(LED4);
	}

}

FRESULT scan_files (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }

    return res;
}



