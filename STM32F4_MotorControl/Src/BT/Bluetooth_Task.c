#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bluetooth_Msg.h"

extern void Receive_TEST();
extern void ProccesReceivedMessage(MyMsg_t *msg);

void Start_BT_Task(void);
static void BT_Task(void *pvParameters);

TaskHandle_t BT_Task_Handle = NULL;
QueueHandle_t xQueueTX;

void Start_BT_Task(void) {
	xTaskCreate(BT_Task, // The function that implements the task.
	"BT_Task", 		// Text name for the task, just to help debugging.
	configMINIMAL_STACK_SIZE * 5, // The size (in words) of the stack that should be created for the task.
	NULL, 	// A parameter that can be passed into the task.  Not used
	configMAX_PRIORITIES - 1, // The priority to assign to the task.  tskIDLE_PRIORITY (which is 0) is the lowest priority.  configMAX_PRIORITIES - 1 is the highest priority.
	&BT_Task_Handle);						// TaskHandle_t * const pxCreatedTask
	if (BT_Task_Handle == NULL)
		Error_Handler();
}

static void BT_Task(void *pvParameters) {
	UNUSED(pvParameters);
	xQueueTX = xQueueCreate(25, sizeof(MyMsg_t*));
	if (xQueueTX == NULL)
		Error_Handler();

	Init_BT_UART_Driver();
	MyMsg_t *msgToSend;
	for (;;) {

		if (xQueueReceive(xQueueTX, &(msgToSend), (TickType_t ) 5)) {
			char msgStr[15] = { 0 };
			strcpy(msgStr, "Begin");
			msgStr[5] = msgToSend->UUID;
			memcpy(&msgStr[6], msgToSend->pData, msgToSend->length);
			BT_UART_Driver.Send((uint8_t*) msgStr, 6 + msgToSend->length);
			free(msgToSend);
		}
		osDelay(10);
	}
}

