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


/**
 * Prototypes
 */
void main(void) __attribute__((noreturn));
void init_mcu(void);





/**
 *******************************************************************
 * Interrupt Service routines
 *******************************************************************
 */
// TIMER0 compare match with OCR0A fires every 75 pulses
ISR(TIMER0_COMPA_vect)
{
    globalXpulses++;  // defined in the model.h
}

// Pint Change Interrupt 0 (pin PB0) fires every pulse (320Hz pulse)
// So we must do the compare (75 pulses == one rotation) ourselves
ISR(PCINT0_vect)
{
    globalYpulsesActual++;
    // 75*2 because PCINT is triggered on both up and down flank.
    if(globalYpulsesActual == 75*2){
        globalYpulses++;
        globalYpulsesActual = 0;
    }
}



/**
 *******************************************************************
 * init_mcu(void) called only once after reset.
 *******************************************************************
 */
void init_mcu(void)
{
    /**
     * INIT data direction register:
     */
    // Outputs
    DDRB =  (1<<OUT_MOTOR_Y2) | (1<<OUT_MOTOR_Z1) | (1<<OUT_MOTOR_Z2);
    // Inputs
    DDRB &= ~((1<<IN_JOYSTICK_LEFT) | (1<<IN_JOYSTICK_DOWN) | (1<<IN_PULSE_MOTOR_Y));
    DDRC &= ~( (1<<IN_POTMETER) | (1<<IN_JOYSTICK_UP) | (1<<IN_JOYSTICK_RIGHT) |\
               (1<<IN_Z_AXIS_OUT) | (1<<IN_Z_AXIS_IN) | (1<<IN_X_AXIS_LIMIT)) ;
    DDRD &= ~((1<<IN_Y_AXIS_LIMIT) | (1<<IN_PULSE_MOTOR_X) );
    
    // Outputs
    DDRD =  (1<<OUT_MOTOR_X1) | (1<<OUT_MOTOR_X2) | (1<<OUT_MOTOR_Y1) | (1<<OUT_LED);
    
    // Enable pullups ; note no pull-up on ADC input!
    PORTB = (1<<IN_JOYSTICK_LEFT) | (1<<IN_JOYSTICK_DOWN) ;
    PORTC = (1<<IN_JOYSTICK_UP) | (1<<IN_JOYSTICK_RIGHT) | (1<<IN_Z_AXIS_OUT) | \
            (1<<IN_Z_AXIS_IN) | (1<<IN_X_AXIS_LIMIT);
    
    // PORTD = (1<<IN_Y_AXIS_LIMIT) | (1<<IN_PULSE_MOTOR_X); // Don't think you can use the T0 with pull-up
    PORTD = (1<<IN_Y_AXIS_LIMIT);
    
    
    /**
     * INIT timer registers:
     * 1MHz clock (internal RC oscillator)
     */
    // Reset everything
    TIMSK0 &= ~( (1<<OCIE0B) | (1<<OCIE0A) | (1<<TOIE0) );
    TCCR0A &= ~( (1<<COM0A1) | (1<<COM0A0) | (1<<COM0B1) | (1<<COM0B0) | \
                  (1<<WGM01)  | (1<<WGM00) );
    TCCR0B &= ~( (1<<FOC0A) | (1<<FOC0B) | (1<<WGM02) | \
                   (1<<CS02) | (1<<CS01)  | (1<<CS00));
    // Set the following
    TCCR0B |= (1<<CS02) | (1<<CS01);    // Clock on T0 pin (falling-edge)
    OCR0A = 75;                         // One  motor rotation causes a CTC match
    TIMSK0 |= (1<<OCIE0A);              // Output compare fires an interrupt
    globalXpulses = 0;

    
    
    /** 
     * INIT pin change interrupts
     */
    //EICRA |= (1<<ISC01); // falling edge INT0
   // EIMSK |= (1<<INT0); // Enable INT0  mistake caused software interrupt with no ISR!!!
    PCICR |= (1<<PCIE0); // Pin change Interrupt enable
    PCMSK0 |= (1<<PCINT0); // Pin Change MASK only on int0
    globalYpulsesActual = 0;
    globalYpulses = 0;
    
    
    /**
     * INIT ADC
     */
    ADCSRA |= (1<<ADEN);
    // ADMUX &= ˜(1 <<ADLAR);
    ADMUX = 0x00;
    
    
    // Set Enable Interrupts
    sei();
    
    /**
     * Put the model into the right starting position
     */
    moveToPickUpPoint();
    
}


/**
 *******************************************************************
 * main run loop; never exits!
 *******************************************************************
 */
void main(void)
{
    uint16_t potmeter;
    uint8_t i;
    
    init_mcu();
    
    /* Just some tests */
    moveZin();
    
#if 1
    // X-as
    for(i = 0; i <= 10; i++){
        moveToPosition(i, 0);
        _delay_ms(100);
    }
    for(i = 10; i > 0; i--){
        moveToPosition(i, 0);
        _delay_ms(100);
    }
    _delay_ms(3000);
    // Y-as
    for(i = 0; i <= 10; i++){
        moveToPosition(0, i);
        _delay_ms(100);
    }
    _delay_ms(3000);
    for(i = 10; i > 0; i--){
        moveToPosition(0, i);
        _delay_ms(100);
    }
    _delay_ms(3000);
    
    // Samen
    for(i = 0; i <= 10; i++){
        moveToPosition(i, i);
        _delay_ms(1000);
    }
    _delay_ms(3000);
    for(i = 10; i > 0; i--){
        moveToPosition(i, i);
        _delay_ms(1000);
    }
#endif
    
    

#if 0
    // X-as
    for(i = 0; i <= 10; i++){
        moveToPositionFast(i, 0);
        _delay_ms(100);
    }
    for(i = 10; i > 0; i--){
        moveToPositionFast(i, 0);
        _delay_ms(100);
    }
    _delay_ms(3000);
    // Y-as
    for(i = 0; i <= 10; i++){
        moveToPositionFast(0, i);
        _delay_ms(100);
    }
    _delay_ms(3000);
    for(i = 10; i > 0; i--){
        moveToPositionFast(0, i);
        _delay_ms(100);
    }
    _delay_ms(3000);
    
    // Samen
    for(i = 0; i <= 10; i++){
        moveToPositionFast(i, i);
        _delay_ms(1000);
    }
    _delay_ms(3000);
    for(i = 10; i > 0; i--){
        moveToPositionFast(i, i);
        _delay_ms(1000);
    }
#endif
    
    
    
    // End of tests
    
    
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
            if((PINC & 1<<IN_Z_AXIS_IN) ? 1: 0){
                motorZturn(IN);
            }
            else{
                motorZoff();
            }
        }
        else if( ( potmeter > ((1024/5) * 1.00) ) &&
                 ( potmeter <= ((1024/5) * 4.00) )) {
            motorZoff();
        }
        else if( potmeter > ((1024/5) * 4.00) ){
            if((PINC & 1<<IN_Z_AXIS_OUT) ? 1: 0){
                motorZturn(OUT);
            }
            else {
                motorZoff();
            }
        }
        
    } /* End of while(1) loop */

}

/* EOF */
