#ifndef __KEY_H__
#define __KEY_H__
#include "stm32f4xx.h"
#define KEY0 1
#define KEY1 2
#define KEY2 3
#define WK_UP 4

typedef enum key_status
{
	KEY_DOWN,
	KEY_UP,
	KEY_ERR
}key_status_typedef;

void key_init(void);
key_status_typedef key_check(int KEYx);
void key_init_int(void);

#endif

