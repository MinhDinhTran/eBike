#ifndef LOG_LOG_H_
#define LOG_LOG_H_

#include "Bluetooth_Msg.h"
#define USE_USB_OTG_LOG

void Init_Log();

typedef struct Log_t Log_t;
struct Log_t
{
    void (*Write)(Bluetooth_MSG_t*);
    void (*WriteByUUID)(uint8_t, int32_t);
};
extern Log_t Log;
#endif /* LOG_LOG_H_ */
