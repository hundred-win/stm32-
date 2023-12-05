#ifndef __MICRO_H
#define __MICRO_H

void microsonic_init(void);
int measure_micro(void);

#define GPIO_MICRO		GPIOC
#define PIN_TRIG		GPIO_Pin_0
#define PIN_ECHO		GPIO_Pin_1

#endif

