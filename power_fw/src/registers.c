/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "registers.h"

#include <stdlib.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <string.h>

#include "twi_slave.h"
#include "board.h"
#include "sys_time.h"
#include "monitor.h"
#include "conf_store.h"
/******************************************************************************
*                                    DATA                                     *
******************************************************************************/
extern volatile uint8_t rebootflag;

static uint8_t registers[ NUM_REGISTERS ];
/******************************************************************************
*                             FUNCTION PROTOTYPES                             *
******************************************************************************/
static void read_time_registers(void);
static void write_time_registers(void);
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
static void read_time_registers(void)
{
	uint32_t seconds;

	memcpy(&seconds, registers + REG_SECONDS_0, sizeof(seconds));
	sys_time_set_time(seconds);
}
/*****************************************************************************/
static void write_time_registers(void)
{
	uint32_t seconds = sys_time_get_time();

	memcpy(registers + REG_SECONDS_0, &seconds, sizeof(seconds));
}
/*****************************************************************************/
// Internal interface
void registers_set_mask( uint8_t index, uint8_t mask )
{
	registers[ index ] |= mask;
}
/*****************************************************************************/
void registers_clear_mask( uint8_t index, uint8_t mask )
{
	registers[ index ] &= ~mask;
}
/*****************************************************************************/
uint8_t registers_get( uint8_t index )
{
	return registers[ index ];
}
/*****************************************************************************/
void registers_set( uint8_t index, uint8_t value )
{
	registers[ index ] = value;
}
/*****************************************************************************/
// Host interface
uint8_t registers_host_read( uint8_t index )
{
	monitor_activity();

	switch ( index )
	{

	case REG_STATUS:
		// Update pgood status
		board_pgood();
		// Update button status
		if ( PIND & PIN_BUTTON ) {
			registers[ REG_STATUS ] &= ~STATUS_BUTTON;
		} else {
			registers[ REG_STATUS ] |= STATUS_BUTTON;
		}
		// Update opto status
		if ( PINB & PIN_OPTO ) {
			registers[ REG_STATUS ] &= ~STATUS_OPTO;
		} else {
			registers[ REG_STATUS ] |= STATUS_OPTO;
		}

		break;
	case REG_SECONDS_0:
		write_time_registers();
		break;
	}

	return registers[ index ];
}
/*****************************************************************************/
void registers_host_write( uint8_t index, uint8_t data )
{
	monitor_activity();

	switch ( index )
	{
	case REG_OSCCAL:
		OSCCAL = data;
		break;

	case REG_CONTROL:
		if ( data & CONTROL_CE ) {
			board_ce( 1 );
		} else {
			board_ce( 0 );
		}

		board_led1(data & CONTROL_LED1);
		board_led2(data & CONTROL_LED2);

		if ( data & CONTROL_BOOTLOAD ) {
			rebootflag = 1;
		}

		break;

	case REG_RESTART_HOURS:
	case REG_RESTART_MINUTES:
	case REG_RESTART_SECONDS:
		registers[ index ] = data;
		return;

	case REG_SECONDS_3:
		registers[index] = data;
		read_time_registers();
		return;

	case REG_EXTENDED:
		// Don't let this register change
		return;

	case REG_I2C_ADDRESS:
		// TODO: qualify address
		store_config( CONF_I2C_ADDR, data );
		break;

	case REG_I2C_ICHARGE:
		if ( data > 3 ) data = 3;
		board_set_charge_current( data );
		store_config( CONF_CHG_CURRENT, data );
		break;

	case REG_I2C_TCHARGE:
		if ( data < 3 ) data = 3;
		if ( data > 10 ) data = 10;
		board_set_charge_timer( data );
		store_config( CONF_CHG_TIMER, data );
		break;
	case REG_MONITOR_CTL:
		conf_store_set_monitor_ctl(data);
		break;
	case REG_MONITOR_BOOT_MINUTES:
		conf_store_set_boot_minutes(data);
		break;
	case REG_START_ENABLE:
		conf_store_set_start_en(data);
		break;
	case REG_MONITOR_RUN_SECONDS:
		conf_store_set_run_seconds(data);
		break;
	case REG_MONITOR_HALT_MINUTES:
		conf_store_set_halt_minutes(data);
		break;
	default:
		break;
	}

	registers[ index ] = data;
}
/*****************************************************************************/
void registers_init( void )
{
	uint8_t t;

	registers[ REG_CONTROL ]            = CONTROL_CE;
	registers[ REG_RESTART_HOURS ]      = 0;
	registers[ REG_RESTART_MINUTES ]    = 0;
	registers[ REG_RESTART_SECONDS ]    = 0;
	registers[ REG_EXTENDED ]           = 0x69;
	registers[ REG_CAPABILITY ]         = CAPABILITY_STATUS;
	registers[REG_MONITOR_CMD]          = MONITOR_CMD_ERR;
	registers[ REG_BOARD_TYPE ]         = conf_store_get_board_type();
	registers[ REG_BOARD_REV ]          = conf_store_get_revision_value();
	registers[ REG_BOARD_STEP ]         = conf_store_get_stepping_value();
	registers[REG_MONITOR_CTL]          = conf_store_get_monitor_ctl();
	registers[REG_MONITOR_BOOT_MINUTES] = conf_store_get_boot_minutes();
	registers[ REG_START_ENABLE ]       = conf_store_get_start_en();
	registers[REG_MONITOR_RUN_SECONDS]  = conf_store_get_run_seconds();
	registers[REG_MONITOR_HALT_MINUTES] = conf_store_get_halt_minutes();

	t = conf_store_get_i2c_address();
	if ( t == 0xFF ) {
		t = TWI_SLAVE_ADDRESS;
	}
	registers[ REG_I2C_ADDRESS ]     = t;

	t = conf_store_get_charge_current();
	if ( t == 0xFF ) {
		t = 1;  // 1/3 amp default
	}
	registers[ REG_I2C_ICHARGE ]     = t;

	t = conf_store_get_charge_timer();
	if ( t == 0xFF ) {
		t = 3;  // 3 hours default
	}
	registers[ REG_I2C_TCHARGE ]      = t;
}
/*****************************************************************************/
