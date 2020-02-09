#include "main.h"

void delay(uint32_t takts);

char tmp;
bool ComReceived = false;
char RxBuffer[BUFF_SIZE];
char TxBuffer[BUFF_SIZE];
uint32_t alarm_start;
uint32_t alarm = 0;


void port_init(void){
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;	// GPIOA Clock ON. Alter function clock ON
	

	GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);		//PA2 на выход
	GPIOA->CRH |= (GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1);

	GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);		//PA3 - вход
	GPIOA->CRH |= GPIO_CRH_CNF10_0;
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	GPIOC->CRH = 0;
	GPIOC->CRL = 0;
	GPIOC->CRL |= GPIO_CRL_MODE7_1;
	GPIOC->CRH |= GPIO_CRH_MODE8_1;
	GPIOC->CRH |= GPIO_CRH_MODE9_1;
}
 
void usart_init(void){
	RCC->APB2ENR	|= RCC_APB2ENR_USART1EN;			// USART1 Clock ON
	USART1->BRR 	= 0xD0;	
		
	USART1->CR2 = 0;
	USART1->CR1 = 0;
	
	USART1->CR1 	|= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;	// USART1 ON, TX ON, RX ON						
	USART1->CR1 |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_IRQHandler(void){
	char tmp = USART1->DR;
	if (tmp > 31 && tmp < 127)		//Прерывание по приёму данных?
		{
			RxBuffer[strlen(RxBuffer)] = tmp;
	 }
	else if (tmp == 10)
		ComReceived = true;
}

void rtc_init(void){
	RCC->CSR |= RCC_CSR_LSION;
	while (~(RCC->CSR & RCC_CSR_LSIRDY)){}
				
	RCC->APB1ENR	|= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
	PWR->CR |= PWR_CR_DBP;
	
	RCC->BDCR |= RCC_BDCR_RTCSEL_LSI;	
	RCC->BDCR |= RCC_BDCR_RTCEN;
		
	RTC->CRL &= ~(0x0008);//Registers Synchronized flag 
  while (RTC->CRL & RTC_FLAG_RSF){}
		
	RTC->CRH |= 0x0002;//ALARM INTERRUPT
	NVIC_EnableIRQ(RTC_IRQn);
	NVIC_SetPriority(RTC_IRQn, 1);
}

void rtc_conf(void){
	if(~(RTC->CRL & RTC_CRL_RTOFF)){
		RTC->CRL |= RTC_CRL_CNF;
			RTC_SetPrescaler(320000 - 1);
			while (RTC->CRL & RTC_FLAG_RTOFF){}
		RTC->CRL &= ~RTC_CRL_CNF;
		while (RTC->CRL & RTC_CRL_RTOFF){};
	}
}

void RTC_IRQHandler(void){
	if (RTC->CRL & (1<<1) ) {                       // check alarm flag
    RTC->CRL &= ~(1<<1);                          // clear alarm flag
    LED_ON();		// Alarm LED on
		alarm_start = alarm;
   }
}

uint32_t str_to_dgt(char *str){
	uint32_t n = 0x1;
	uint32_t al = 0x0;
	for (int i = strlen(str); i >0 ; i--)
		if (str[i] >= '0' && str[i] <= '9'){
			al += (str[i] - '0')*n;
			n *= 10;
		}
		USART1->DR = al;
		return al;
}

void command(void){
	memset(TxBuffer, 0, TX_BUFF_SIZE);
	char s[BUFF_SIZE];
	
	if (strncmp(RxBuffer,"ALARM",5) == 0){
		alarm = str_to_dgt(RxBuffer);
	}
	RTC->CRL |= RTC_CRL_CNF;
		RTC->ALRH = alarm >> 16;
		RTC->ALRL = (alarm & RTC_LOWER_BITS);
		while (RTC->CRL & RTC_FLAG_RTOFF){}
	RTC->CRL &= ~(RTC_CRL_CNF);
	
	ComReceived = false;
	memset(RxBuffer,0,RX_BUFF_SIZE);
}

int main(void)
{
	uint32_t  time;
	port_init();
	usart_init();
	rtc_init();
	rtc_conf();

	RTC->CRL |= RTC_CRL_CNF;
		RTC->CNTH = 0;
		RTC->CNTL = 0;
		while (RTC->CRL & RTC_FLAG_RTOFF){}
	RTC->CRL &= ~RTC_CRL_CNF;
	
	while(1)
 {
	 time = RTC->CNTL | (RTC->CNTH << 16 );
	 if (ComReceived){
			command();
		}
		if (time == alarm_start + 0x05)
			LED_OFF();
	}
 
 }

void delay(uint32_t takts){
	uint32_t i;
	
	for (i = 0; i < takts; i++) {};
}
