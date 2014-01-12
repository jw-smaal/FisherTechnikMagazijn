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
    //TCCR1B |= ( (1<<CS10) | (1<<CS11) ); // Use prescale divided by 64.
    
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
    uint16_t potmeter= 0;
    
    init_mcu();
    
    // main run loop never exit!
    while(1) {
        switch(joystickState()){
        case JOYSTICK_STATE_CENTRE:
            motorXoff();
            motorYoff();
                break;
        case JOYSTICK_STATE_LEFT:
            motorXturn(LEFT);
                break;
        case JOYSTICK_STATE_RIGHT:
            motorXturn(RIGHT);
                break;
        case JOYSTICK_STATE_UP:
            motorYturn(UP);
                break;
        case JOYSTICK_STATE_DOWN:
            motorYturn(DOWN);
                break;
        default:
            motorXoff();
            motorYoff();
            break;
        }

        // Move Z-axis based on Potmeter reading.
        potmeter = readPotMeter();
        if(potmeter < ((1024/5) * 1.00)){
            motorZturn(IN);
        }
        else if( ( potmeter > ((1024/5) * 1.00) ) &&
                 ( potmeter <= ((1024/5) * 4.00) )) {
            motorZoff();
        }
        else if( potmeter > ((1024/5) * 4.00) ){
            motorZturn(OUT);
            
        }
        
    } /* End of while(1) loop */

}

/* EOF */
