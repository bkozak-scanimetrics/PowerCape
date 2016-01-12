#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "board.h"
#include "eeprom.h"
#include "registers.h"
#include "twi_slave.h"
#include "bb_i2c.h"
#include "board_power.h"
#include "board_watchdog.h"


extern volatile uint16_t system_ticks;
volatile uint8_t rebootflag = 0;

uint8_t mcusr __attribute__ ((section (".noinit")));

void get_mcusr(void) __attribute__((naked)) __attribute__((section(".init3")));
void get_mcusr( void )
{
    mcusr = MCUSR;
    MCUSR = 0;
    wdt_enable( WDTO_2S );
}

// If requested, make sure CE is set. Can be disabled by ISR.
void check_charge_enable( void )
{
    if ( registers_get( REG_CONTROL ) & CONTROL_CE )
    {
        board_ce( 1 );
        board_enable_pgood_irq();
    }
}


int main( void )
{
    uint8_t oscval;
    uint16_t last_tick = 0;

    // Make sure DIV8 is not selected
    if ( CLKPR != 0 )    // Div1
    {
        CLKPR = ( 1 << CLKPCE );
        CLKPR = 0;
    }

    // Platform setup
    board_init();
    registers_init();
    registers_set( REG_MCUSR, mcusr );

    oscval = eeprom_get_calibration_value();
    if ( oscval != 0xFF )
    {
        OSCCAL = oscval;
    }
    else
    {
        oscval = OSCCAL;
    }
    registers_set( REG_OSCCAL, oscval );

    set_sleep_mode( SLEEP_MODE_PWR_SAVE );
    sei();

    board_set_charge_current( registers_get( REG_I2C_ICHARGE ) );
    board_set_charge_timer( registers_get( REG_I2C_TCHARGE ) );

    // Main loop
    while ( 1 )
    {
        wdt_reset();

        // System ticks are seconds
        if ( last_tick != system_ticks )
        {
            last_tick = system_ticks;

            board_power_sm();
            if (board_power_state_is_on())
            {
                board_watchdog_check();
            } else if (board_power_state_is_off()) {
                sleep_enable();
                sleep_cpu();
                sleep_disable();
                TCCR2A = 0;
            }

            check_charge_enable();
        }

        // Bootloader entry
        if ( rebootflag != 0 )
        {
            twi_slave_stop();
            board_stop();
            eeprom_set_bootloader_flag();
            cli();
            wdt_enable( WDTO_30MS );
            while ( 1 );
        }

        // Register handling
        if ( registers_get( REG_OSCCAL ) != oscval )
        {
            oscval = registers_get( REG_OSCCAL );
            eeprom_set_calibration_value( oscval );
            OSCCAL = oscval;
        }
    }
}
