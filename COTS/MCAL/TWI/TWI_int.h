#ifndef TWI_INT_H_
#define TWI_INT_H_

#include "../../../DEFS/STD_types.h"
#include "../../../DEFS/MCU.h"
#include "../../../DEFS/BIT_Math.h"

#include "TWI_priv.h"
#include "TWI_config.h"

STD_Type MCAL_TWI_u8Init(u8 LOC_u8SlaveAddress);	//Assign slave address to the device (0 for no address)
#if( TWI_INTERRUPT == TWI_INTERRUPT_ON )
STD_Type MCAL_TWI_u8CallbackSendTo(u8 LOC_u8SlaveAddress, u8 LOC_u8Data[],void (*LOC_Pu8Ptr)(void));
STD_Type MCAL_TWI_u8CallbackRequestFrom(u8 LOC_u8SlaveAddress,u8 LOC_u8RequestCount,void (*LOC_Pu8Ptr)(void));
#endif
STD_Type MCAL_TWI_u8GetData(u8 * LOC_Pu8TWIData);
STD_Type MCAL_TWI_u8StoreForRequest(u8 LOC_u8Data[]);
#endif /* TWI_INT_H_ */
