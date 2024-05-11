#ifndef TWI_CONFIG_H_
#define TWI_CONFIG_H_

#include "TWI_priv.h"

//OPTIONS:
/*
 * TWI_INTERRUPT_OFF
 * TWI_INTERRUPT_ON
 */
#define TWI_INTERRUPT	TWI_INTERRUPT_ON

//OPTIONS:
//Numbers are acceptable, as long as you write besides it "UL"
//e.g. 40000UL	40 KHz
#define TWI_FREQUENCY	40000UL

//OPTIONS:
/*
 * 1
 * 4
 * 16
 * 64
 */
#define TWI_PRESCALER	16

#define TWI_CPU_FREQ	16000000UL	//MUST match microcontroller's frequency

#endif /* TWI_CONFIG_H_ */
