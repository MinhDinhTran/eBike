#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usbd_cdc_if.h"
#include "log.h"

static uint8_t get_UUID_Index(uint8_t uuid);

static void WriteToLog(Bluetooth_MSG_t* msg);
static void WriteToLogByUUID(uint8_t uuid, int32_t value);

Log_t Log;
QueueHandle_t *xQueue;
#define LOG_CACHE_LEN 5
int32_t log_cache[LOG_CACHE_LEN][5];
uint8_t log_cache_cnt[LOG_CACHE_LEN] = {0};
uint8_t log_cache_ids[LOG_CACHE_LEN] = {0};

#ifdef USE_USB_OTG_LOG
QueueHandle_t xQueue_USB_TX;
#endif

void Init_Log(){
	Log.Write = WriteToLog;
	Log.WriteByUUID = WriteToLogByUUID;
#ifdef USE_USB_OTG_LOG
	xQueue = &xQueue_USB_TX;
#endif
}

static void WriteToLog(Bluetooth_MSG_t* msg){
#ifdef USE_USB_OTG_LOG
	if (xQueue == NULL)
		return;

	if (msg == NULL)
		return;

	xQueueSendFromISR(*xQueue, (void * ) &msg, (TickType_t ) 3);
#endif
}

static void WriteToLogByUUID(uint8_t uuid, int32_t value){
	uint8_t id = get_UUID_Index(uuid);

	log_cache[id][log_cache_cnt[id]++] = value;
	if (log_cache_cnt[id] < 5)
		return;
	log_cache_cnt[id] = 0;
	Bluetooth_MSG_t *msg = malloc(sizeof(*msg));//+ 5*sizeof(int32_t));
	if (msg == NULL)
		return;


	msg->UUID = uuid;//PWM_Lv_UUID;
	msg->length = 5;
	memcpy(msg->MSG, log_cache[id], 5*sizeof(int32_t));

	WriteToLog(msg);
}

static uint8_t get_UUID_Index(uint8_t uuid){
	for(uint8_t i = 0; i<LOG_CACHE_LEN; i++){
		if (log_cache_ids[i] == uuid){
			return i;
		}
		if (log_cache_ids[i] == 0){
			log_cache_ids[i] = uuid;
			return i;
		}
	}
	return LOG_CACHE_LEN;
}










