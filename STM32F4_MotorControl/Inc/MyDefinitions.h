#ifndef MYDEFINITIONS_H_
#define MYDEFINITIONS_H_

#include <../Middlewares/cJSON/cJSON.h>


typedef struct AMessage
 {
    char ucMessageID;
    char ucData[35];
 } xMessage;
#endif /* MYDEFINITIONS_H_ */
