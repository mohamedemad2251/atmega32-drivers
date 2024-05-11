#ifndef TIMER_H_
#define TIMER_H_

#include "../../../DEFS/BIT_Math.h"
#include "../../../DEFS/STD_Types.h"
#include "../../../DEFS/MCU.h"
#include "TIMER_config.h"

#define TIMER0 0
#define TIMER1 1
#define TIMER2 2

STD_Type MCAL_TIMER_u8Init(u8 LOC_u8TimerNo);

#if( (TIMER0_INTERRUPT == TIMER_INTERRUPT_ON) || (TIMER1_INTERRUPT == TIMER_INTERRUPT_ON) )
#if( (TIMER0_MODE == NORMAL) || (TIMER1_MODE == NORMAL) || (TIMER0_MODE == CTC) || (TIMER1_MODE == CTC) )
STD_Type MCAL_TIMER_u8CallbackMilliFun(u8 LOC_u8TimerNo,u32 LOC_u32MilliSeconds,void (*LOC_Pu8Ptr)(void));
STD_Type MCAL_TIMER_u8CallbackMicroFun(u8 LOC_u8TimerNo,u32 LOC_u32MicroSeconds,void (*LOC_Pu8Ptr)(void));
#endif
#endif
#if( TIMER0_INTERRUPT == TIMER_INTERRUPT_OFF || TIMER1_INTERRUPT == TIMER_INTERRUPT_OFF)
#if( TIMER0_MODE == NORMAL || TIMER1_MODE == NORMAL || TIMER2_MODE == NORMAL || TIMER0_MODE == CTC || TIMER1_MODE == CTC|| TIMER2_MODE == CTC)
STD_Type MCAL_TIMER_u8DelayMilliSeconds(u8 LOC_u8TimerNo,u32 LOC_u32MilliSeconds,u8 * LOC_Pu8TimeDone);
#endif
#endif
#if( TIMER0_MODE == PWM || TIMER0_MODE == PHASE_CORRECT || TIMER2_MODE == PWM || TIMER2_MODE == PHASE_CORRECT)
STD_Type MCAL_TIMER_u8SetPWM(u8 LOC_u8TimerNo,u8 LOC_u8Percentage);
#endif
#if( TIMER1_MODE == PWM || TIMER1_MODE == PHASE_CORRECT )
STD_Type MCAL_TIMER_u8SetTIMER1PWM(u8 LOC_u8Percentage,u8 LOC_u8Frequency);
#endif
#if( ICU_MODE == ICU_ON )
STD_Type MCAL_TIMER_u8ICUInit(void);
STD_Type MCAL_TIMER_u8ICUGetDutyFreq(u8 * LOC_Pu8DutyCycle,u32 * LOC_Pu32Frequency);
#endif

#endif /* TIMER_H_ */
