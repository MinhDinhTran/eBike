#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <string.h>
#include "MyMsg.h"

#ifndef CC2541
#include "main.h" // for MyMsg_Error_Handler();
#endif


#define UUID_SIZE 1
#define AFFIX_SIZE 2
#define AFFIX_1 0x0A
#define AFFIX_2 0x0D
#define AFFIX 0x0A0D
#define MSG_LEN_INDICATOR_SIZE 4

#define MYMSG_CACHE_FRAGMENT_SIZE 16
#define MYMSG_CACHE_AFFIX_LOC_CNT_PER_FRAG 4
static void DeleteFirstFragment();

typedef struct MyMsgCache_t MyMsgCache_t;
struct MyMsgCache_t {
	uint8_t dataCount;
	char data[MYMSG_CACHE_FRAGMENT_SIZE];
	int8_t AffixLocs[MYMSG_CACHE_AFFIX_LOC_CNT_PER_FRAG];
	MyMsgCache_t* NextFragment;
};
static MyMsgCache_t* _pCache = NULL;
static MyMsgCache_t* _pLastFragment = NULL;

static uint8_t flag_AffixSearch = 0;
static MyMsgCache_t* flag_NewAffixHead = NULL;
static uint8_t count_AffixesFound = 0;
void MyMsg_Error_Handler() {
#ifndef CC2541
	Error_Handler();
#else
	printf("MyMsg Error\n");
#endif
}
static MyMsgCache_t* CreateCacheFragment() {
	MyMsgCache_t* fragment = malloc(sizeof(MyMsgCache_t));
	if (fragment == NULL)
	{
            MyMsg_Error_Handler();
            return NULL;
        }
	fragment->dataCount = 0;
	memset(fragment->AffixLocs, -1, MYMSG_CACHE_AFFIX_LOC_CNT_PER_FRAG);
	fragment->NextFragment = NULL;
	return fragment;
}
static void DeleteFirstFragment() {   
        if (_pCache == NULL) return;
	if (_pCache->NextFragment != NULL) {
		MyMsgCache_t* pNextFrag = _pCache->NextFragment;
		free(_pCache);
		if (pNextFrag == NULL)
			_pCache = CreateCacheFragment();
		else
			_pCache = pNextFrag;
	} else {
		free(_pCache);
		_pCache = NULL;
	}
}

void MyMsg_CacheStringPiece_ISR(char s) {
	if (_pCache == NULL) {
		_pCache = CreateCacheFragment();
		_pLastFragment = _pCache;
	}
        if (_pCache == NULL)
          return;
	_pLastFragment->data[_pLastFragment->dataCount++] = s;
	switch (flag_AffixSearch) {
	case 0:
		if (s == AFFIX_1) {
			flag_AffixSearch = 1;
			flag_NewAffixHead = _pLastFragment;
		} else
			flag_AffixSearch = 0;
		break;
	case 1:
		flag_AffixSearch = s == AFFIX_2 ? 2 : 0;
		break;
	case 2:
		if (s == AFFIX_1)
			flag_AffixSearch = 3;
		else {
			flag_AffixSearch = 0xF3;
			goto FOUND_AFFIX;
		}
		break;
	case 3:

		if (s == AFFIX_2)
			flag_AffixSearch = 0;
		else {
			flag_AffixSearch = 0xF4;
			goto FOUND_AFFIX;
		}
		break;
	case 0xF3:
	case 0xF4:
		FOUND_AFFIX: count_AffixesFound++;
		for (uint8_t i = 0; i < MYMSG_CACHE_AFFIX_LOC_CNT_PER_FRAG; i++) {

			if (flag_NewAffixHead->AffixLocs[i] == -1) {
				flag_NewAffixHead->AffixLocs[i] = flag_NewAffixHead->dataCount - (flag_AffixSearch & 0x0F) + (
						flag_NewAffixHead->NextFragment != NULL ? flag_NewAffixHead->NextFragment->dataCount : 0);
				break;
			}
		}
		flag_AffixSearch = 0;
		break;
	default:
		break;
	}

	if (_pLastFragment->dataCount >= MYMSG_CACHE_FRAGMENT_SIZE) {
		_pLastFragment->NextFragment = CreateCacheFragment();
		_pLastFragment = _pLastFragment->NextFragment;
                if (_pLastFragment == NULL)
                {
                    MyMsg_Error_Handler();
                    return ;
                }
	}
}

MyMsg_t* MyMsg_ProcessCache() {
	if (count_AffixesFound < 2)
		return NULL;
	uint16_t msgLen = 0;
	int8_t* affix1_Loc = NULL;
	int8_t* affix2_Loc = NULL;
	MyMsgCache_t* cache_MsgTail = NULL;
	for (uint8_t i = 0; i < MYMSG_CACHE_AFFIX_LOC_CNT_PER_FRAG; i++) {
		if (_pCache->AffixLocs[i] >= 0) {
			affix1_Loc = &_pCache->AffixLocs[i];
			break;
		}
	}
	if (affix1_Loc == NULL) {
		DeleteFirstFragment();
		for (uint8_t i = 0; i < MYMSG_CACHE_AFFIX_LOC_CNT_PER_FRAG; i++) {
			if (_pCache->AffixLocs[i] >= 0) {
				affix1_Loc = &_pCache->AffixLocs[i];
				break;
			}
		}
	}
	for (uint8_t i = affix1_Loc - _pCache->AffixLocs + 1; i < MYMSG_CACHE_AFFIX_LOC_CNT_PER_FRAG; i++) {
		if (_pCache->AffixLocs[i] >= 0) {
			affix2_Loc = &(_pCache->AffixLocs[i]);
			break;
		}
	}

	// message can occupie more than 2 fragments?
	uint8_t fragCount = 0;
	if (affix2_Loc == NULL) {
		MyMsgCache_t* pNextFrag = _pCache->NextFragment;
		while (cache_MsgTail == NULL) {
			if (pNextFrag == NULL)
				return NULL;
			fragCount++;
			for (uint8_t i = 0; i < MYMSG_CACHE_AFFIX_LOC_CNT_PER_FRAG; i++) {
				if (pNextFrag->AffixLocs[i] >= 0) {
					affix2_Loc = &(pNextFrag->AffixLocs[i]);
					cache_MsgTail = pNextFrag;
					break;
				}
			}
			pNextFrag = pNextFrag->NextFragment;
		}

	}
	if (affix2_Loc == NULL)
		return NULL; // should not be possible, because checking count_AffixesFound beforehand
	if (cache_MsgTail == NULL) // if whole msg is in single frag
		msgLen = *affix2_Loc - *affix1_Loc;
	else
		msgLen = (fragCount - 1) * MYMSG_CACHE_FRAGMENT_SIZE + MYMSG_CACHE_FRAGMENT_SIZE - *affix1_Loc + *affix2_Loc - 1;
	msgLen += AFFIX_SIZE;
	if (msgLen <= MSG_LEN_INDICATOR_SIZE + UUID_SIZE) {
		*affix1_Loc = -2;
		count_AffixesFound--;
		return NULL;
	}

	MyMsg_t* result = NULL;
	result = malloc(sizeof(MyMsg_t));
	if (result == NULL)
        {
            MyMsg_Error_Handler();
            return  NULL;
        }

	if (cache_MsgTail == NULL) {
		memcpy(&result->length, &_pCache->data[*affix1_Loc + AFFIX_SIZE], MSG_LEN_INDICATOR_SIZE);
		result->UUID = _pCache->data[AFFIX_SIZE + MSG_LEN_INDICATOR_SIZE];

		if (result->length <= MSG_LEN_INDICATOR_SIZE + UUID_SIZE) {
			*affix1_Loc = -2;
			count_AffixesFound--;
			return NULL;
		}
		result->pData = malloc(result->length);
		if (result->pData == NULL)
                {
                    MyMsg_Error_Handler();
                    return NULL;
                }
		memcpy(result->pData, &_pCache->data[*affix1_Loc + AFFIX_SIZE + MSG_LEN_INDICATOR_SIZE + UUID_SIZE], result->length);
		if (*affix1_Loc + 2 * AFFIX_SIZE + MSG_LEN_INDICATOR_SIZE + UUID_SIZE + result->length > MYMSG_CACHE_FRAGMENT_SIZE)
			DeleteFirstFragment();
	} else {
		char* tail = NULL;
		if (MYMSG_CACHE_FRAGMENT_SIZE - *affix1_Loc > AFFIX_SIZE + MSG_LEN_INDICATOR_SIZE) {
			tail = &_pCache->data[*affix1_Loc + AFFIX_SIZE];
			memcpy(&result->length, tail, MSG_LEN_INDICATOR_SIZE);
			tail += MSG_LEN_INDICATOR_SIZE;
		} else {
			result->length = 0;
			uint8_t temp = 0;

			tail = &_pCache->data[*affix1_Loc];
			//AFFIX_SIZE
			for (temp = 0; tail != &_pCache->data[MYMSG_CACHE_FRAGMENT_SIZE];) {
				tail++;
				temp++;
				if (temp == AFFIX_SIZE)
					break;
			}
			if (tail == &_pCache->data[MYMSG_CACHE_FRAGMENT_SIZE]) {
				DeleteFirstFragment();
				tail = _pCache->data;
			}
			if (temp != AFFIX_SIZE) {
				for (; tail != &_pCache->data[MYMSG_CACHE_FRAGMENT_SIZE];) {
					tail++;
					temp++;
					if (temp == AFFIX_SIZE)
						break;
				}
			}

			//MSG_LEN_INDICATOR_SIZE
			if (tail == &_pCache->data[MYMSG_CACHE_FRAGMENT_SIZE]) {
				DeleteFirstFragment();
				tail = _pCache->data;
			}
			for (temp = 0; tail != &_pCache->data[MYMSG_CACHE_FRAGMENT_SIZE];) {
				result->length |= *tail << (temp * 8);
				tail++;
				temp++;
				if (temp == MSG_LEN_INDICATOR_SIZE)
					break;
			}
			if (tail == &_pCache->data[MYMSG_CACHE_FRAGMENT_SIZE]) {
				DeleteFirstFragment();
				tail = _pCache->data;
			}
			if (temp != MSG_LEN_INDICATOR_SIZE) {
				for (; tail != &_pCache->data[MYMSG_CACHE_FRAGMENT_SIZE];) {
					result->length |= *tail << (temp * 8);
					tail++;
					temp++;
					if (temp == MSG_LEN_INDICATOR_SIZE)
						break;
				}
			}
		}

		if (tail == &_pCache->data[MYMSG_CACHE_FRAGMENT_SIZE]) {
			DeleteFirstFragment();
			tail = _pCache->data;
		}
		result->UUID = *tail++;
		if (tail == &_pCache->data[MYMSG_CACHE_FRAGMENT_SIZE]) {
			DeleteFirstFragment();
			tail = _pCache->data;
		}
		if (result->length <= MSG_LEN_INDICATOR_SIZE + UUID_SIZE || msgLen +1 != result->length) {
			*affix1_Loc = -2;
			count_AffixesFound--;
			return NULL;
		}
		//TODO: MyMsg data can occupie more than 2 fragments. need add check
		result->pData = malloc(result->length);
		if (result->pData == NULL)
		{
                    MyMsg_Error_Handler();
                    return NULL;
                }
		uint32_t dataCopied = 0;
		uint8_t dataInThisFrag = &_pCache->data[MYMSG_CACHE_FRAGMENT_SIZE] - tail;
		if (dataInThisFrag > result->length)
			dataInThisFrag = result->length;
		memcpy(result->pData, tail, dataInThisFrag);
		dataCopied += dataInThisFrag;

		if (dataCopied != result->length) {
			DeleteFirstFragment();
			tail = _pCache->data;
			memcpy(&((char*) (result->pData))[dataInThisFrag], tail, result->length - dataCopied);
		}

	}
	*affix1_Loc = -2;
	*affix2_Loc = -2;
	count_AffixesFound -= 2;
	return result;
}

// need add validation for outside usage

MyMsg_t* MyMsg_ParseFragments(char* str, uint32_t len, uint8_t containsAffix) {
	MyMsg_t* msg = malloc(sizeof(MyMsg_t));
	if (msg == NULL)
	{
            MyMsg_Error_Handler();
            return NULL;
        }

	memcpy(&msg->length, &str[AFFIX_SIZE], sizeof(uint32_t));
	msg->UUID = str[AFFIX_SIZE + MSG_LEN_INDICATOR_SIZE];
	msg->pData = malloc(msg->length);
	if (msg->pData == NULL)
	{
            MyMsg_Error_Handler();
            return NULL;
        }
	memcpy(msg->pData, &str[AFFIX_SIZE + MSG_LEN_INDICATOR_SIZE + UUID_SIZE], msg->length);

	return msg;
}

MyMsg_t* MyMsg_CreateString(uint8_t UUID, void* pData, uint32_t len) {

	char* str = (char*) pData;
	uint32_t* locations = malloc(sizeof(uint32_t));
	if (locations == NULL)
	{
            MyMsg_Error_Handler();
            return NULL;
        }
	uint32_t locationsCount = 0;

	char* pAffix = memchr(str, AFFIX, len);
	char* pPrevAffix = str;
	while (pAffix != NULL) {
		locationsCount++;
		if (locationsCount > 1)
			locations = realloc(locations, locationsCount * sizeof(uint32_t));
		locations[locationsCount - 1] = pAffix - str;
		uint32_t bytesLeftToScan = len - (pAffix + AFFIX_SIZE - pPrevAffix);
		pPrevAffix = pAffix + AFFIX_SIZE;
		pAffix = memchr(pPrevAffix, AFFIX, bytesLeftToScan);
	}

	uint32_t wholeMsgLen = len + MSG_LEN_INDICATOR_SIZE + UUID_SIZE + (AFFIX_SIZE * (locationsCount + 2));
	char* pStr = malloc(wholeMsgLen + 1);
	if (pStr == NULL) {
            MyMsg_Error_Handler();
            return NULL;
        }
	pStr[0] = AFFIX_1;
	pStr[1] = AFFIX_2;
	memcpy(&pStr[2], &wholeMsgLen, 4);
	pStr[6] = UUID;

	char* pStrTail = &pStr[7];
	if (locationsCount == 0) {
		memcpy(pStrTail, str, len);
		pStrTail += len;
	} else {
		for (uint32_t i = 0; i < locationsCount; i++) {
			uint32_t start = i == 0 ? 0 : locations[i - 1];
			uint32_t end = locations[i];
			uint32_t chrCountToCopy = end - start;
			memcpy(pStrTail, &str[start], chrCountToCopy);
			pStrTail += chrCountToCopy;
			*pStrTail++ = AFFIX_1;
			*pStrTail++ = AFFIX_2;
		}
		uint32_t start = locations[locationsCount - 1];
		uint32_t end = len;
		uint32_t chrCountToCopy = end - start;
		memcpy(pStrTail, &str[start], chrCountToCopy);
		pStrTail += chrCountToCopy;
	}
	pStrTail[0] = AFFIX_1;
	pStrTail[1] = AFFIX_2;
	pStrTail[2] = 0; // will be checksum or something. also need at least 1 char after msg
	free(locations);
	MyMsg_t* msg = malloc(sizeof(MyMsg_t));
        if (msg == NULL){
            MyMsg_Error_Handler();
            return NULL;
        }
	msg->UUID = 0xFF;
	msg->length = wholeMsgLen;
	msg->pData = pStr;

	return msg;
}

