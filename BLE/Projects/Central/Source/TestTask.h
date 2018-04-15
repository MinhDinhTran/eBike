#ifndef TESTTASK_H
#define TESTTASK_H
#include "hal_types.h"

void TestTask_Init( uint8 task_id );
uint16 TestTask_ProcessEvent( uint8 task_id, uint16 events );

#endif