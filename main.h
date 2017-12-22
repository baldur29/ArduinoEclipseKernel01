/*
 * main.h
 *
 *  Created on: 28. nóv. 2016
 *      Author: baldurtho
 */

#ifndef MAIN_H_
#define MAIN_H_

#define LEDDIR DDRB
#define LEDPORT PORTB
#define PIN_LED (1<<5)

#define LED_INIT LEDDIR |= PIN_LED
#define LED_ON LEDPORT |= PIN_LED
#define LED_OFF LEDPORT &= ~PIN_LED
#define LED_TOGGLE LEDPORT ^= PIN_LED

#define SST_USE_SIGNAL_MASK 1
void SST_Start(void);
void SST_OnIdle(void);

#endif /* MAIN_H_ */
