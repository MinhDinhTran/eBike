#ifndef MY_MSG_H_
#define MY_MSG_H_

#ifdef CC2541
  //#include <stdint.h>  // have no idea why IAR cant find this file... opens it, tho. everything ok with stdbool.h & other stds............ 
  //using C99,    Options-> C/C++ Compilier -> Language 1
  //Options-> C/C++ Compilier -> Proprocessor ->ingnote standard library include .. unchecked
  // defininions specially for IAR workspace
  #define int8_t __INT8_T_TYPE__
  #define uint8_t __UINT8_T_TYPE__
  #define uint16_t __UINT16_T_TYPE__
  #define uint32_t __UINT32_T_TYPE__
#endif
typedef struct MyMsg_t MyMsg_t;
struct MyMsg_t {
	uint8_t UUID;
	uint32_t length;
	void* pData;
};
MyMsg_t* MyMsg_CreateString(uint8_t UUID, void* pData, uint32_t len);
void MyMsg_CacheStringPiece_ISR(char s);
MyMsg_t* MyMsg_ProcessCache();

#endif /* MY_MSG_H_ */

