#ifndef DC_MOTOR_INT_H_
#define DC_MOTOR_INT_H_

#include "../../MCAL/DIO/DIO.h"
#include "../../MCAL/TIMER/TIMER.h"
#include "../../MCAL/TIMER/TIMER_config.h"

#define DC_MOTOR0	0
#define DC_MOTOR1	1
#define DC_MOTOR2	2

#define DC_MOTOR_FORWARD	0
#define DC_MOTOR_BACKWARD	1

#if(TIMER0_MODE == PWM || TIMER0_MODE == PHASE_CORRECT || TIMER1_MODE == PWM || TIMER1_MODE == PHASE_CORRECT || TIMER2_MODE == PWM || TIMER2_MODE == PHASE_CORRECT)
STD_Type HAL_DC_MOTOR_u8Init(u8 LOC_u8MotorNo);
STD_Type HAL_DC_MOTOR_u8AdjustSpeedPercentage(u8 LOC_u8MotorNo,u8 LOC_u8SpeedPercentage);
STD_Type HAL_DC_MOTOR_u8AdjustDirection(u8 LOC_u8MotorNo,u8 LOC_u8Direction);
#endif

#endif /* DC_MOTOR_INT_H_ */
