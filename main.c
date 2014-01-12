/**
 *******************************************************************
 * $Id:$
 * FISHERTECHNIK model 
 *
 * Jan-Willem Smaal <jws@gispen.org> 22 Nov 2013
 *******************************************************************
 */
#include <inttypes.h>
#include <avr/signature.h>
#include <avr/io.h>
#include <avr/interrupt.h>
// For the <util/delay.h> function in this case the internal 1MHz clock is used.
#define F_CPU 1000000UL
#include <util/delay.h>

/* Implementation of fishertechnik model */
#include "FisherTechnikMagazijnModel.h"


void main(void) __attribute__((noreturn));

/**
 *******************************************************************
 * init_mcu(void) called only once after reset.
 *******************************************************************
 */
void init_mcu(void)
{
    /*
     * INIT data direction register:
     */
    DDRB =  (1<<OUT_MOTOR_Y2) | (1<<OUT_MOTOR_Z1) | (1<<OUT_MOTOR_Z2);
    DDRB &= ~((1<<IN_JOYSTICK_LEFT) | (1<<IN_JOYSTICK_DOWN));
    DDRC &= ~( (1<<IN_POTMETER) | (1<<IN_JOYSTICK_UP) | (1<<IN_JOYSTICK_RIGHT) ) ;
    DDRD =  (1<<OUT_MOTOR_X1) | (1<<OUT_MOTOR_X2) | (1<<OUT_MOTOR_Y1) | (1<<OUT_LED);
    
    
    PORTB = (1<<IN_JOYSTICK_LEFT) | (1<<IN_JOYSTICK_DOWN) ;
    PORTC = (1<<IN_JOYSTICK_UP) | (1<<IN_JOYSTICK_RIGHT) ;  // Note: no pull-up on ADC-input!
    PORTD = 0;
    
    /*
     * INIT timer registers:
     * 1MHz clock / 64 = 15,625 KHz
     * 16bit TCNT1 counter overflows every (15,625 KHz/65535) = 0.23 times per second.
     */
    TCCR1B |= ( (1<<CS10) | (1<<CS11) ); // Use prescale divided by 64.
    
    /*
     * INIT ADC
     */
    ADCSRA |= (1<<ADEN);
    // ADMUX &= ˜(1 <<ADLAR);
    ADMUX = 0x00;
    
    
    
#if 0
    // This ledsequence is to indicate power-on-reset.
    allOff();
    for(i = 0; i < 5; i++){
        ledOn();
        _delay_ms(100);
        ledOff();
        _delay_ms(100);
    }
#endif 
    
}


/** 
 *******************************************************************
 * main run loop; never exits!
 *******************************************************************
 */
void main(void)
{
    
    init_mcu();
    
    // main run loop never exit!
    while(1) {

        if(  ((PINB & 1<<IN_JOYSTICK_LEFT) ? 1: 0)  \
           && ((PINC & 1<<IN_JOYSTICK_RIGHT) ? 1: 0) \
           && ((PINC & 1<<IN_JOYSTICK_UP) ? 1: 0)\
           && ((PINB & 1<<IN_JOYSTICK_DOWN) ? 1: 0)){
            allOff();
        }
        else {
            if( (PINB & (1<<IN_JOYSTICK_LEFT)) ? 0: 1 ) {
                motorXturn(LEFT);
            }
            if((PINC & (1<<IN_JOYSTICK_RIGHT)) ? 0: 1) {
                motorXturn(RIGHT);
            }
            if((PINC & (1<<IN_JOYSTICK_UP)) ? 0: 1) {
                motorYturn(UP);
            }
            if((PINB & (1<<IN_JOYSTICK_DOWN)) ? 0: 1) {
                motorYturn(DOWN);
            }
        }
        
        if(readPotMeter() > (1024/)
    }
}

#if 0
        // X-as
        motorXturn(LEFT);
        _delay_ms(500);
        motorXturn(RIGHT);
        _delay_ms(500);
        motorXoff();
      
        // Y-as
        motorYturn(UP);
        _delay_ms(500);
        motorYturn(DOWN);
        _delay_ms(500);
        motorYoff();
        
        // Z-as
        motorZturn(IN);
        _delay_ms(500);
        motorZturn(OUT);
        _delay_ms(500);
        motorZoff();
     
#endif
/* EOF */
