#include "TWI_priv.h"
#include "TWI_config.h"
#include "TWI_address.h"
#include "TWI_int.h"

#include "../../HAL/LCD/LCD.h"

#if( TWI_INTERRUPT == TWI_INTERRUPT_OFF || TWI_INTERRUPT == TWI_INTERRUPT_ON )
#if( TWI_INTERRUPT == TWI_INTERRUPT_OFF )
#define TWIE_MODE	(0<<TWIE)
#elif( TWI_INTERRUPT == TWI_INTERRUPT_ON )
#define	TWIE_MODE	(1<<TWIE)
#endif
#else
#error "Wrong configuration for TWI_INTERRUPT, please re-configure!"
#endif

#if( TWI_PRESCALER == 1 || TWI_PRESCALER == 4 || TWI_PRESCALER == 16 || TWI_PRESCALER == 64 )
#if( TWI_PRESCALER == 1 )
#define	TWPS_MODE	( (0<<TWPS1)| (0<<TWPS0) )
#elif( TWI_PRESCALER == 4 )
#define	TWPS_MODE	( (0<<TWPS1)| (1<<TWPS0) )
#elif( TWI_PRESCALER == 16 )
#define	TWPS_MODE	( (1<<TWPS1)| (0<<TWPS0) )
#elif( TWI_PRESCALER == 64 )
#define	TWPS_MODE	( (1<<TWPS1)| (1<<TWPS0) )
#endif
#else
#error "Wrong configuration for TWI_PRESCALER, please re-configure!"
#endif

#if( TWI_FREQUENCY > ZERO )
#define F_RATIO		(TWI_CPU_FREQ/TWI_FREQUENCY)	//Ratio between F_CPU & F_SCL chosen
#else
#error "Freq_SCL cannot be zero!"
#endif
#if( F_RATIO < 16 )	//Corner Case: From equation, it can lead to a negative number in TWBR
#error "Freq_CPU must be at least 16 times bigger than Freq_SCL!"
#else
#define TWBR_NO_TWPS	((F_RATIO-16)/2)			//Applying the Freq_SCL equation WITHOUT the 4^(TWPS) (Because we need a for loop for it)
#endif

#define TWPS_MULTIPLIER 4							//4^(TWPS)

#if( TWI_INTERRUPT == TWI_INTERRUPT_ON )
static u8 * GLOB_Pu8TWISendData;
static u8 GLOB_u8TWIRecieveData;
static u8 GLOB_u8SlaveAddress;
static u8 GLOB_u8TWIStatus;		//Status code returned by hardware
static u8 GLOB_u8TWIDirection = TWI_WRITE;	//Current transmission direction. This resource saves that state.
static u8 GLOB_u8TWIRequestCount;			//Number of requests of data to be read from slave and sent to master.
static u8 GLOB_u8TWIDataPending = FALSE;
static u8 GLOB_u8TWIDataReceived = TRUE;
static void (*GLOB_PvoidTWIGptr)(void) = NULL;
#endif

STD_Type MCAL_TWI_u8Init(u8 LOC_u8SlaveAddress)	//Assign slave address to the device (0 for no address)
{
	STD_Type LOC_u8ReturnValue = E_NOT_OK;
	u8 LOC_u8TWPSValue = ONE;
	u8 LOC_u8TWBRValue = ZERO;
	u8 LOC_u8Iterations = ZERO;

	for(LOC_u8Iterations;LOC_u8Iterations<TWPS_MODE;LOC_u8Iterations++)
	{
		LOC_u8TWPSValue = LOC_u8TWPSValue * TWPS_MULTIPLIER;
	}
	TWBR = (TWBR_NO_TWPS/LOC_u8TWPSValue);
	TWCR = TWIE_MODE;	//Enable/Disable PIE (Depends on configuration)
	SET_BIT(TWCR,TWEA);	//Enable acknowledgement bit
	TWSR = TWPS_MODE;	//Only the TWPS bits 1 & 0, the rest is read-only/reserved
	if(LOC_u8SlaveAddress > ZERO)
	{
		TWAR = LOC_u8SlaveAddress << ONE;	//Shifted by 1 because the 0th bit is used for general call
	}
	#if( TWI_INTERRUPT == TWI_INTERRUPT_ON )
	SET_BIT(SREG,I_BIT);	//Enable GIE
	#endif
	SET_BIT(TWCR,TWEN);	//Finally, enable the TWI Peripheral
	LOC_u8ReturnValue = E_OK;

	return LOC_u8ReturnValue;
}

#if( TWI_INTERRUPT == TWI_INTERRUPT_ON )
static STD_Type MCAL_TWI_u8SendData(u8 LOC_u8TWIData)
#elif( TWI_INTERRUPT == TWI_INTERRUPT_OFF )
STD_Type MCAL_TWI_u8SendData(u8 LOC_u8TWIData)
#endif
{
	STD_Type LOC_u8ReturnValue = E_NOT_OK;

	TWDR = LOC_u8TWIData;

	LOC_u8ReturnValue = E_OK;
	return LOC_u8ReturnValue;
}

#if( TWI_INTERRUPT == TWI_INTERRUPT_ON )
static STD_Type MCAL_TWI_u8RecieveData(u8 * LOC_Pu8TWIData)
#elif( TWI_INTERRUPT == TWI_INTERRUPT_OFF )
STD_Type MCAL_TWI_u8RecieveData(u8 * LOC_Pu8TWIData)
#endif
{
	STD_Type LOC_u8ReturnValue = E_NOT_OK;

	*LOC_Pu8TWIData = TWDR;

	LOC_u8ReturnValue = E_OK;
	return LOC_u8ReturnValue;
}

STD_Type MCAL_TWI_u8GetData(u8 * LOC_Pu8TWIData)
{
	STD_Type LOC_u8ReturnValue = E_NOT_OK;
	// 	HAL_LCD_u8WriteInteger(GLOB_u8TWIRecieveData);
	// 	HAL_LCD_u8WriteChar(' ');
	if(LOC_Pu8TWIData && (GLOB_u8TWIDataReceived == FALSE))
	{
		*LOC_Pu8TWIData = GLOB_u8TWIRecieveData;
		GLOB_u8TWIDataReceived = TRUE;
		GLOB_u8TWIRecieveData = ZERO;
		#if (TWI_INTERRUPT == TWI_INTERRUPT_ON)
		// 		if(GLOB_u8TWIStatus == TWI_SLAVE_REC_DATA_ACK_OK)
		// 		{
		// 			SET_BIT(TWCR,TWINT);	//When reading data, reset the interrupt flag
		// 		}
		#endif
	}
	else
	{
		*LOC_Pu8TWIData = ZERO;
	}

	LOC_u8ReturnValue = E_OK;
	return LOC_u8ReturnValue;
}

STD_Type MCAL_TWI_u8StoreForRequest(u8 LOC_u8Data[])
{
	STD_Type LOC_u8ReturnValue = E_NOT_OK;
	if(LOC_u8Data && GLOB_u8TWIDataPending == FALSE)
	{
		GLOB_Pu8TWISendData = LOC_u8Data;
	}
	else
	{
		// Do Nothing.
	}
	
	return LOC_u8ReturnValue;
}

#if( TWI_INTERRUPT == TWI_INTERRUPT_ON )
STD_Type MCAL_TWI_u8CallbackSendTo(u8 LOC_u8SlaveAddress, u8 LOC_u8Data[],void (*LOC_Pu8Ptr)(void))
{
	STD_Type LOC_u8ReturnValue = E_NOT_OK;
	if( LOC_u8Data && (LOC_Pu8Ptr != NULL) )
	{
		if(GLOB_u8TWIDataPending == FALSE)
		{
			GLOB_u8SlaveAddress = LOC_u8SlaveAddress;
			GLOB_Pu8TWISendData = LOC_u8Data;
			GLOB_PvoidTWIGptr = LOC_Pu8Ptr;
			GLOB_u8TWIDataPending = TRUE;

			GLOB_u8TWIDirection = TWI_WRITE;
			CLEAR_BIT(TWCR,TWSTA);
			SET_BIT(TWCR,TWSTA);	//Enable the START Condition
		}
		else
		{
			//Do Nothing.
		}
		LOC_u8ReturnValue = E_OK;
	}
	else
	{
		//Do Nothing.
	}
	return LOC_u8ReturnValue;
}

STD_Type MCAL_TWI_u8CallbackRequestFrom(u8 LOC_u8SlaveAddress,u8 LOC_u8RequestCount,void (*LOC_Pu8Ptr)(void))
{
	STD_Type LOC_u8ReturnValue = E_NOT_OK;
	if(LOC_u8RequestCount > ZERO && LOC_Pu8Ptr != NULL)
	{
		GLOB_u8SlaveAddress = LOC_u8SlaveAddress;
		GLOB_u8TWIRequestCount = LOC_u8RequestCount;
		GLOB_PvoidTWIGptr = LOC_Pu8Ptr;

		GLOB_u8TWIDirection = TWI_READ;
		SET_BIT(TWCR,TWEA);		//Enable Acknowledgement bit, because it can get disabled in the ISR.
		SET_BIT(TWCR,TWSTA);	//Enable the START Condition
		
		LOC_u8ReturnValue = E_OK;
	}
	else
	{
		//Do Nothing.
	}
	return LOC_u8ReturnValue;
}

STD_Type MCAL_TWI_u8CallbackRecieveOnly(void (* LOC_PvoidPtr)(void))
{
	STD_Type LOC_u8ReturnValue = E_NOT_OK;
	
	if(LOC_PvoidPtr != NULL)
	{
		GLOB_PvoidTWIGptr = LOC_PvoidPtr;
		LOC_u8ReturnValue = E_OK;
	}
	else
	{
		//Do Nothing.
	}
	return LOC_u8ReturnValue;
}

#endif

#if( TWI_INTERRUPT == TWI_INTERRUPT_ON )
ISR(TWI_vect)
{
	GLOB_u8TWIStatus = TWSR & TWI_MASK_STATUS;
	
	//HAL_LCD_u8WriteInteger(GLOB_u8TWIStatus);
	//HAL_LCD_u8WriteChar(' ');
	
	switch(GLOB_u8TWIStatus)
	{
		case TWI_START_DONE:
		if(GLOB_u8TWIDirection == TWI_READ)
		{
			TWDR = (GLOB_u8SlaveAddress << ONE) + TWI_READ;	//Send to the slave we're targeting + read bit
		}
		else if(GLOB_u8TWIDirection == TWI_WRITE)
		{
			TWDR = (GLOB_u8SlaveAddress << ONE) + TWI_WRITE;	//Send to the slave we're targeting + write bit
		}
		break;
		case TWI_MSTR_SLA_W_ACK_OK:
		if(GLOB_Pu8TWISendData && *GLOB_Pu8TWISendData)
		{
			TWDR = *GLOB_Pu8TWISendData;
			GLOB_Pu8TWISendData++;
		}
		break;
		case TWI_MSTR_SLA_W_ACK_NOK:
		if(GLOB_Pu8TWISendData)
		{
			CLEAR_BIT(TWCR,TWSTA);
			SET_BIT(TWCR,TWSTA);
			SET_BIT(TWCR,TWSTO);
		}
		else
		{
			GLOB_Pu8TWISendData = ZERO;
			GLOB_u8SlaveAddress = ZERO;
			GLOB_u8TWIDataPending = FALSE;
			CLEAR_BIT(TWCR,TWSTA);
			SET_BIT(TWCR,TWSTO);
		}
		break;
		case TWI_MSTR_TRN_DATA_ACK_OK:
		if(GLOB_Pu8TWISendData && *GLOB_Pu8TWISendData && GLOB_u8TWIDataPending)
		{
			TWDR = *GLOB_Pu8TWISendData;
			GLOB_Pu8TWISendData++;
		}
		else
		{
			CLEAR_BIT(TWCR,TWSTA);	//Added this, might remove it later
			SET_BIT(TWCR,TWSTO);
			GLOB_Pu8TWISendData = ZERO;
			GLOB_u8SlaveAddress = ZERO;
			GLOB_u8TWIDataPending = FALSE;
		}
		break;
		case TWI_MSTR_TRN_DATA_ACK_NOK:
		if(GLOB_Pu8TWISendData && GLOB_u8TWIDataPending)
		{
			SET_BIT(TWCR,TWSTA);
			CLEAR_BIT(TWCR,TWSTO);
		}
		else
		{
			GLOB_Pu8TWISendData = ZERO;
			GLOB_u8SlaveAddress = ZERO;
			GLOB_u8TWIDataPending = FALSE;
			CLEAR_BIT(TWCR,TWSTA);
			SET_BIT(TWCR,TWSTO);	//Set STOP condition and try again
		}
		break;
		case TWI_MSTR_SLA_R_ACK_OK:
		//Do Nothing.
		break;
		case TWI_MSTR_SLA_R_ACK_NOK:
		CLEAR_BIT(TWCR,TWSTA);
		SET_BIT(TWCR,TWSTO);	//Set STOP condition and try again
		SET_BIT(TWCR,TWSTA);	//Try again via START condition (must win arbitration again)
		break;
		case TWI_MSTR_REC_DATA_ACK_OK:
		if(GLOB_u8TWIRequestCount > ZERO)
		{
			GLOB_u8TWIRecieveData = TWDR;
			GLOB_u8TWIRequestCount--;
			GLOB_u8TWIDataReceived = FALSE;
		}
		else
		{
			CLEAR_BIT(TWCR,TWEA);	//Remove the acknowledgement, so send NOT ACK to the slave, stopping will be another step
		}
		break;
		case TWI_MSTR_REC_DATA_ACK_NOK:
		CLEAR_BIT(TWCR,TWSTA);
		SET_BIT(TWCR,TWSTO);	//Set STOP condition
		if(GLOB_u8TWIRequestCount > ZERO)
		{
			SET_BIT(TWCR,TWSTA);	//This actually means the issue was on the slave-side, so the master still requests more data
		}
		break;
		case TWI_SLAVE_REC_DATA_ACK_OK:
		GLOB_u8TWIRecieveData = TWDR;
		GLOB_u8TWIDataReceived = FALSE;
		break;
		case TWI_SLAVE_REC_DATA_ACK_NOK:
		GLOB_u8TWIRecieveData = TWDR;
		GLOB_u8TWIDataReceived = FALSE;
		if(GLOB_u8SlaveAddress > ZERO)	//The only indication that CallbackSendTo() was used and this slave is actually trying to send/recieve something as MSTR
		{
			CLEAR_BIT(TWCR,TWSTO);
			SET_BIT(TWCR,TWSTA);	//Try to become master next clock by starting
		}
		break;
		case TWI_SLAVE_STOP:
		if(GLOB_u8SlaveAddress > ZERO)	//The only indication that CallbackSendTo() was used and this slave is actually trying to send/recieve something as MSTR
		{
			CLEAR_BIT(TWCR,TWSTO);
			SET_BIT(TWCR,TWSTA);	//Try to become master next clock by starting
		}
		break;
		case TWI_ARBIT_LOST:
		CLEAR_BIT(TWCR,TWSTO);
		SET_BIT(TWCR,TWSTA);	//Start again (Try to obtain the arbitration next time)
		break;
		case TWI_SLAVE_SLA_R_ACK_OK:
			if(GLOB_Pu8TWISendData && *GLOB_Pu8TWISendData)
			{
				TWDR = *GLOB_Pu8TWISendData;
				GLOB_Pu8TWISendData++;
			}
			break;
		case TWI_SLAVE_ARBIT_LOST:
			//Do Nothing.
			break;
		case TWI_SLAVE_TRN_DATA_ACK_OK:
			if(GLOB_Pu8TWISendData && *GLOB_Pu8TWISendData)
			{
				TWDR = *GLOB_Pu8TWISendData;
				GLOB_Pu8TWISendData++;
			}
			break;
		case TWI_SLAVE_TRN_DATA_ACK_NOK:
			if(GLOB_u8SlaveAddress)
			{
				CLEAR_BIT(TWCR,TWSTA);
				SET_BIT(TWCR,TWSTA);
				GLOB_u8TWIDataPending = TRUE;
			}
			break;
		case TWI_SLAVE_TRN_LAST_DATA_ACK_OK:
			if(GLOB_u8SlaveAddress)
			{
				CLEAR_BIT(TWCR,TWSTA);
				SET_BIT(TWCR,TWSTA);
				GLOB_u8TWIDataPending = TRUE;
			}
			break;
	}
	if(GLOB_PvoidTWIGptr != NULL)
	{
		GLOB_PvoidTWIGptr();
	}
	if(GLOB_u8TWIDataReceived == TRUE)
	{
		SET_BIT(TWCR,TWINT);			//The TWI Interrupt flag is exclusively not cleared by hardware in ISR
		//So we clear it ourselves because as long as it's HIGH, the SCL extends the low period interrupting the TWI.
	}	//NOTE: IF the data hasn't been used yet by the user, I'm not allowing the I2C to continue.
}
#endif