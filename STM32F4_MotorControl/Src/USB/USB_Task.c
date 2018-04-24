#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usbd_cdc_if.h"
#include "Bluetooth_Msg.h"


void Start_USB_Task(void);
static void USB_Task(void *pvParameters);

TaskHandle_t USB_Task_Handle = NULL;
QueueHandle_t xQueue_USB_TX;

void Start_USB_Task(void) {
	USB_Task_Handle = (TaskHandle_t) xTaskCreate(USB_Task,// The function that implements the task.
			"USB_Task", 		// Text name for the task, just to help debugging.
			configMINIMAL_STACK_SIZE, // The size (in words) of the stack that should be created for the task.
			NULL, 	// A parameter that can be passed into the task.  Not used
			configMAX_PRIORITIES-2,// The priority to assign to the task.  tskIDLE_PRIORITY (which is 0) is the lowest priority.  configMAX_PRIORITIES - 1 is the highest priority.
			NULL);						// TaskHandle_t * const pxCreatedTask

}

static void USB_Task(void *pvParameters) {
	UNUSED(pvParameters);
	xQueue_USB_TX = xQueueCreate(10, sizeof(MyMsg_t*));
	if (xQueue_USB_TX == NULL)
		Error_Handler();

	//uint8_t str[50];
	//MyMsg_t *msgToSend;

	for (;;) {
		/*if (xQueueReceive(xQueue_USB_TX, &(msgToSend), (TickType_t ) 5)) {
			uint8_t len = UUID_LEN;
			str[MSG_LEN_SIZE] = msgToSend->UUID;
			str[MSG_LEN_SIZE+1] = 0;//dummy
			str[MSG_LEN_SIZE+2] = 0;//dummy
			len+=2;
			uint8_t l = sizeof(msgToSend->pMsg) * msgToSend->length;
			memcpy(&str[len+MSG_LEN_SIZE], msgToSend->pMsg, l);
			len += l;


			len += sprintf((char*)&str[len+MSG_LEN_SIZE], "\r\n");

			len += MSG_LEN_SIZE;
			str[0] = len & 0xff;

			free(msgToSend);
			CDC_Transmit_FS((uint8_t*) str, (int)len);
		}*/
	}
}

