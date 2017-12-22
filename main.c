/*
 * main.c
 *
 *  Created on: 28. nóv. 2016
 *      Author: baldurtho
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <sst_timer.h>
#include "main.h"


//http://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html#details
//https://www.embedded.com/design/prototyping-and-development/4025691/Build-a-Super-Simple-Tasker
static int uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,_FDEV_SETUP_WRITE);


#define TIMER_TASK_PRIO        (SST_MIN_TASK_PRIO + 1)
#define EVENT_TASK_PRIO        (SST_MIN_TASK_PRIO + 2)

#if defined(SST_USE_SIGNAL_MASK) && SST_USE_SIGNAL_MASK != 0
#define LED_MASK    8
#endif

SST_TaskID_T timer_task_id;
SST_TaskID_T event_task_id;

SST_TaskID_T timer_task_id_Task1;
SST_TimedTask_T* ttask_Task1;

SST_TaskID_T timer_task_id_Task2;
SST_TimedTask_T* ttask_Task2;


#if SST_MAX_EV_QUEUE_LEN == 0
SST_EVENTQ_DEFINE(t_Task1_queue, 2);
SST_EVENTQ_DEFINE(t_Task2_queue, 2);
#endif /* SST_MAX_EV_QUEUE_LEN == 0 */



//struct SST_Ticker_Tag ticker;
void TickerBussyCallback(void);
void TickerIdleCallback(void);

SST_TICKER_DEFINE(ticker, 1, TickerBussyCallback, TickerIdleCallback);
SST_EVENTQ_DEFINE(t_10ms_queue, 2);

static int uart_putchar(char c, FILE *stream)
{
  while(!(UCSR0A & (1<<UDRE0)))
  	;//wait for ready
  UDR0 = c;
  return 0;
}



void USARTInit(uint16_t baud)
{
   //Set Baud rate
   UBRR0=(int)(F_CPU/16/baud-1);

   /*Set Frame Format
   >> Asynchronous mode
   >> No Parity
   >> 1 StopBit
   >> char size 8 bit
   */
   UCSR0C|=(1<<UCSZ00)|(1<<UCSZ01);

   //Enable The receiver and transmitter
   UCSR0B|=(1<<RXEN0)|(1<<TXEN0);

}

void IoInit()
{
	TCCR0A=0; //normal mode
	TCCR0B=0x05 ; // f=16MHz/1024
	OCR0A=156;	//10ms ticks
	TIMSK0=0x02; //output compare interrupt enable
	TIFR0=0x02; //clear flags
	/*
	TCNT0 = T0_INIT;
	    // set CK/TDIV prescaler for timer 0
	    TCCR0 = T0CLKDIV;

	    TIMSK = _BV(TOIE0);

	    TCCR1A = 0;
	    TCCR1B = _BV(CTC1);
	    OCR1A = T1_INIT;
	    TIMER100MS_ON();
	 */
}

int main()
{
	LED_INIT; //make pin output (1)
	//init_uart();
	stdout = &mystdout;
	USARTInit(9600);
	printf("startup, ");


/*
	while(1)
	{
		LED_ON;
		_delay_ms(1000);
		LED_OFF;
		printf("hello-");
		_delay_ms(1000);
	}
*/

	SST_Run();
	return (int)0;
}

// this is an ISR function for periodic timer interrupt
ISR(TIMER0_COMPA_vect)
{
    SST_DECLARE_ISR;
    SST_ISR_ENTRY(SST_MIN_INT_PRIO+1);
    SST_TimerTick(&ticker);
    SST_ISR_EXIT();
    //LED_TOGGLE;
}

void TimerTask1_func(SST_Event_T e)
{
	printf("TASK1, ");
}

void TimerTask2_func(SST_Event_T e)
{
	printf("TASK2, ");
}


void SST_Start(void)
{
	printf("initialization, ");
	IoInit();
	#if SST_MAX_EV_QUEUE_LEN == 0
		timer_task_id_Task1 = SST_CreateTask(TIMER_TASK_PRIO, TimerTask1_func, &t_Task1_queue);
		timer_task_id_Task2 = SST_CreateTask(TIMER_TASK_PRIO, TimerTask2_func, &t_Task2_queue);
	#else
		timer_task_id_Task1 = SST_CreateTask(TIMER_TASK_PRIO, TimerTask1_func);
		timer_task_id_Task2 = SST_CreateTask(TIMER_TASK_PRIO, TimerTask2_func);
	#endif

	(void) SST_MakeTimedTask(timer_task_id_Task1, 20, true, &ticker);
	(void) SST_MakeTimedTask(timer_task_id_Task2, 40, true, &ticker);
	SST_INT_UNLOCK();

}

void SST_OnIdle(void)
{
	printf("idle");
	_delay_ms(1000);
}


void TickerBussyCallback(void)
{
	printf("TickerBussyCallback, ");
}
void TickerIdleCallback(void)
{
	printf("TickerIdleCallback, ");
}
