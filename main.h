#ifndef __MAIN_H
#define	__MAIN_H

#include "stm32f10x.h" 
#include "stdbool.h"
#include "string.h"

#define BUFF_SIZE 256

#define RX_BUFF_SIZE 256
#define TX_BUFF_SIZE 256

#define DELAY_VAL 250
#define MAX_DEL 500
#define MIN_DEL 50
#define STEP 50

#define RTC_LOWER_BITS    0x0000FFFF  /*!< RTC LSB Mask */

#define	LED_ON()	GPIOC->BSRR |= GPIO_BSRR_BS7
#define	LED_OFF()	GPIOC->BSRR |= GPIO_BSRR_BR7

#define	LED1_ON()	GPIOC->BSRR |= GPIO_BSRR_BS8
#define	LED1_OFF()	GPIOC->BSRR |= GPIO_BSRR_BR8

#define	LED2_ON()	GPIOC->BSRR |= GPIO_BSRR_BS9
#define	LED2_OFF()	GPIOC->BSRR |= GPIO_BSRR_BR9

#define	LED1_IS_ON()	(GPIOC->ODR & GPIO_ODR_ODR8) > 0	
#define	LED2_IS_ON()	(GPIOC->ODR & GPIO_ODR_ODR9) > 0

void port_init(void);
void usart_init(void);
void USART1_IRQHandler(void);
void rtc_init(void);
void rtc_conf(void);
uint32_t str_to_dgt(char *str);
void command(void);
void delay(uint32_t takts);

#endif