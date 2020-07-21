#ifndef __BEEP_H__
#define __BEEP_H__
#include "stm32f4xx.h"
#include "bitband.h"
#define BEEP PFout(8)

void beep_init(void);

#endif
