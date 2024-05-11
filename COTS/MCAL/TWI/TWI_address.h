#ifndef TWI_ADDRESS_H_
#define TWI_ADDRESS_H_

#if( TWI_INTERRUPT == TWI_INTERRUPT_ON )
#define TWI_vect	__vector_19
#endif

#define TWBR 	(*(volatile u8*) 0x20)

#define TWCR	(*(volatile u8*) 0x56)
#define	TWINT	Pin7
#define TWEA	Pin6
#define	TWSTA	Pin5
#define	TWSTO	Pin4
#define	TWWC	Pin3
#define	TWEN	Pin2
#define	TWIE	Pin0	//Pin 1 is reserved

#define TWSR	(*(volatile u8*) 0x21)
#define	TWS7	Pin7
#define TWS6	Pin6
#define	TWS5	Pin5
#define	TWS4	Pin4
#define	TWS3	Pin3
#define	TWPS1	Pin1
#define	TWPS0	Pin0	//Pin 2 is reserved

#define TWDR	(*(volatile u8*) 0x23)

#define TWAR	(*(volatile u8*) 0x22)
#define	TWA6	Pin7
#define TWA5	Pin6
#define	TWA4	Pin5
#define	TWA3	Pin4
#define	TWA2	Pin3
#define TWA1	Pin2
#define	TWA0	Pin1
#define	TWGCE	Pin0

#endif /* TWI_ADDRESS_H_ */
