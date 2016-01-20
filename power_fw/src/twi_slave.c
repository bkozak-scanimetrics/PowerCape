/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "twi_slave.h"

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "registers.h"
/******************************************************************************
*                                    DATA                                     *
******************************************************************************/
static volatile uint8_t data_count;
static volatile uint8_t reg_index;
/******************************************************************************
*                         SLAVE RECIEVER STATUS CODES                         *
******************************************************************************/
/* Own SLA+W has been received; ACK has been returned */
#define TWI_S_RCVR_STATE1 0X60
/* Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has
   been returned */
#define TWI_S_RCVR_STATE2 0x68
/* General call address has been received; ACK has been returned */
#define TWI_S_RCVR_STATE3 0x70
/* Arbitration lost in SLA+R/W as Master; General call address has been
   received; ACK has been returned */
#define TWI_S_RCVR_STATE4 0x78
/* Previously addressed with own SLA+W; data has been received;
   ACK has been returned */
#define TWI_S_RCVR_STATE5 0x80
/* Previously addressed with own SLA+W; data has been received; NOT ACK has
   been returned */
#define TWI_S_RCVR_STATE6 0x88
/* Previously addressed with general call; data has been re- ceived;
   ACK has been returned  */
#define TWI_S_RCVR_STATE7 0x90
/*  Previously addressed with general call; data has been received; NOT ACK
   has been returned */
#define TWI_S_RCVR_STATE8 0x98
/* A STOP condition or repeated START condition has been received while still
   addressed as Slave */
#define TWI_S_RCVR_STATE9 0xA0
/******************************************************************************
*                       SLAVE TRANSMITTER STATUS CODES                        *
******************************************************************************/
/* Own SLA+R has been received; ACK has been returned */
#define TWI_S_TRMR_STATE1 0xA8
/* Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK
   has been returned */
#define TWI_S_TRMR_STATE2 0xB0
/* Data byte in TWDR has been transmitted; ACK has been received */
#define TWI_S_TRMR_STATE3 0xB8
/* Data byte in TWDR has been transmitted; NOT ACK has been received */
#define TWI_S_TRMR_STATE4 0xC0
/* Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been
   received */
#define TWI_S_TRMR_STATE5 0xC8
/******************************************************************************
*                              MISC STATUS CODES                              *
******************************************************************************/
/* No relevant state information available; TWINT = “0 */
#define TWI_IDLE      0xF8
/* Bus error due to an illegal START or STOP condition */
#define TWI_BUS_ERROR 0x00
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
ISR( TWI_vect )
{
	uint8_t data;
	uint8_t status = TWSR & 0xFC;

	switch ( status )
	{
		case TWI_S_RCVR_STATE1:  // SLA+W
		case TWI_S_RCVR_STATE9:  // Stop or repeated start
		{
			data_count = 0;
			break;
		}

		case TWI_S_TRMR_STATE1:  // SLA+R
		case TWI_S_TRMR_STATE3:  // Data sent + ACK
		{
			TWDR = registers_host_read( reg_index++ );

			if ( reg_index >= NUM_REGISTERS )
			{
				reg_index = 0;
			}

			break;
		}

		case TWI_S_RCVR_STATE5:  // data RX
		{
			data = TWDR;
			TWCR |= ( 1 << TWINT );

			if ( data_count == 0 )
			{
				reg_index = data;
			}
			else
			{
				registers_host_write( reg_index++, data );
			}

			if ( reg_index >= NUM_REGISTERS )
			{
				reg_index = 0;
			}

			data_count++;
			return;
		}

		case TWI_S_TRMR_STATE4:  // data TX + NAK
		case TWI_S_TRMR_STATE5:  // last data (TWEA=0) + ACK
		{
			break;
		}
		default:
		{
			break;
		}
	}

	// Last thing is to clear the INT flag
	TWCR |= ( 1 << TWINT );
}
/*****************************************************************************/
void twi_slave_init( void )
{
	uint8_t i;

	i = registers_get( REG_I2C_ADDRESS );
	if ( i & 0x80 )
	{
		i = TWI_SLAVE_ADDRESS;
	}

	TWAR = ( i << 1 );
	TWCR = ( 1 << TWEA ) | ( 1 << TWEN ) | ( 1 << TWIE );
}
/*****************************************************************************/
void twi_slave_stop( void )
{
	TWCR = 0;
}
/*****************************************************************************/
