#ifndef TWI_PRIV_H_
#define TWI_PRIV_H_

#define TWI_INTERRUPT_OFF	0
#define TWI_INTERRUPT_ON	1

//TWI Status (Developer)
#define TWI_PENDING	0
#define TWI_START	1
#define TWI_ADDRESS	2
#define TWI_DATA_MASTER	3
#define TWI_DATA_SLAVE	4
#define TWI_STOP		5

#define TWI_WRITE	0
#define TWI_READ	1

#define TWI_MASK_STATUS	0xF8	//Mask the first 3 bits (DON'T SHIFT THEM AFTERWARDS)

//TWI Status (Datasheet/Hardware)
//General Status Codes (Master & Slave)
#define TWI_START_DONE				0x08
#define TWI_REP_START_DONE			0x10
#define TWI_ARBIT_LOST				0x38

//Master Transmitter Status Codes
#define TWI_MSTR_SLA_W_ACK_OK		0x18
#define TWI_MSTR_SLA_W_ACK_NOK		0x20
#define TWI_MSTR_TRN_DATA_ACK_OK	0x28
#define TWI_MSTR_TRN_DATA_ACK_NOK	0x30

//Master Receiver Status Codes
#define TWI_MSTR_SLA_R_ACK_OK		0x40
#define TWI_MSTR_SLA_R_ACK_NOK		0x48
#define TWI_MSTR_REC_DATA_ACK_OK	0x50
#define TWI_MSTR_REC_DATA_ACK_NOK	0x58

//Slave Transmitter Status Codes
#define TWI_SLAVE_SLA_R_ACK_OK		0xA8
#define TWI_SLAVE_ARBIT_LOST		0xB0
#define TWI_SLAVE_TRN_DATA_ACK_OK	0xB8
#define TWI_SLAVE_TRN_DATA_ACK_NOK	0xC0
#define TWI_SLAVE_TRN_LAST_DATA_ACK_OK	0xC8

//Slave Receiver Status Codes
#define TWI_SLAVE_REC_DATA_ACK_OK		0x80
#define TWI_SLAVE_REC_DATA_ACK_NOK		0x88
#define TWI_SLAVE_STOP					0xA0


#endif /* TWI_PRIV_H_ */
