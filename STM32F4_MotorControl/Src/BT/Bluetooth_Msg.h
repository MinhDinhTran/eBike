#ifndef BT_BLUETOOTH_MSG_H_
#define BT_BLUETOOTH_MSG_H_

#include "cmsis_os.h"
#include "MyMsg.h"


#define UUID_LEN 1
#define MSG_LEN_SIZE 1

#define MODE_ID                         0
#define PWM_DUTY_CYCLE_ID               1
#define V_THRESHOLD_ID                  2
#define BIKE_BATTERY_LEVEL_ID           3
#define CURRENT_ID                      4
#define BIKE_SPEED_ID                   5
#define BIKE_FLAGS_ID                   6


#define MODE_LEN                        1
#define PWM_DUTY_CYCLE_LEN              1
#define V_THRESHOLD_LEN                 4
#define BIKE_BATTERY_LEVEL_LEN          2
#define CURRENT_LEN                     2
#define BIKE_SPEED_LEN                  4
#define BIKE_FLAGS_LEN                  4

typedef struct UART_Driver UART_Driver;
struct UART_Driver
{
    void (*RxCpltCallback)(void);
    void (*TxCpltCallback)(void);
    uint32_t (*Send)(uint8_t*, uint16_t);
    TaskHandle_t *xTaskToNotify;
};


extern QueueHandle_t xQueueTX;
extern UART_Driver BT_UART_Driver;
void Init_BT_UART_Driver(void);

#endif /* BT_BLUETOOTH_MSG_H_ */
