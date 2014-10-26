/**
 *******************************************************************
 * $Id:$
 * FISHERTECHNIK model: Magazijn Robot 
 *
 * Jan-Willem Smaal <jws@gispen.org> 12 Jan 2014
 *******************************************************************
 */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
// For the <util/delay.h> function in this case the internal 1MHz clock is used.
#define F_CPU 1000000UL
#include <util/delay.h>

#include "FisherTechnikMagazijnModel.h"

/**
 *******************************************************************
 * VARIOUS FisherTechnik related functions for controlling the model
 * most return void because there is nothing to check 
 * just reading/writing to memory registers.
 *******************************************************************
 */
void flashntimes(uint8_t n){
    uint8_t i;
    for (i = 0; i < n; i++){
        ledOn();
        _delay_ms(300);
        ledOff();
        _delay_ms(200);
    }
}

void allOff()
{
    ledOff();
    motorXoff();
    motorYoff();
    motorZoff();
}


void moveZin()
{
    while((PINC & 1<<IN_Z_AXIS_IN) ? 1: 0){
        motorZturn(IN);
    }
    motorZoff();
}

void moveZout()
{
    while((PINC & 1<<IN_Z_AXIS_OUT) ? 1: 0){
        motorZturn(OUT);
    }
    motorZoff();
}


void moveToPickUpPoint()
{
    uint8_t xlimitreached;
    uint8_t ylimitreached;
    uint8_t positionreached;
  
    
    moveZin();
    
    xlimitreached = (PINC & 1<<IN_X_AXIS_LIMIT) ? 0: 1;
    ylimitreached = (PIND & 1<<IN_Y_AXIS_LIMIT) ? 0: 1;
    positionreached = xlimitreached && ylimitreached;
    
    while(!positionreached){
        if(!xlimitreached) {
            motorXturn(RIGHT);
        }
        else {
            motorXoff();
        }
        
        if(!ylimitreached){
            motorYturn(DOWN);
        }
        else {
            motorYoff();
        }
        xlimitreached = (PINC & 1<<IN_X_AXIS_LIMIT) ? 0: 1;
        ylimitreached = (PIND & 1<<IN_Y_AXIS_LIMIT) ? 0: 1;
        positionreached = xlimitreached && ylimitreached;
    }
    allOff();  // May not have reached X or Y off so...
    moveZout();
    globalXposition = 0;
    globalXpulses = 0;
    globalYposition = 0;
    globalYpulses = 0;
    globalYpulsesActual = 0;
    //allOff();
}


/**
 * This function can be optimized by starting 
 * two motors at the same time.
 */
void moveToPosition(uint8_t x, uint8_t y){
    motorXmoveToPosition(x);
    motorYmoveToPosition(y);
}

// Broken function starts osscillating !!!
void moveToPositionFast(uint8_t x, uint8_t y){
    uint8_t positionreached = 0;
    uint8_t xoffset = 0;
    uint8_t yoffset = 0;
    uint8_t xdirection = NONE;
    uint8_t ydirection = NONE;
    
    if (globalYposition == x && globalYposition == y){
        return;
    }

    globalXpulses = 0;
    globalYpulses = 0;
    globalYpulsesActual = 0;
    
    
    while(!positionreached) {
        /* X-axis */
        if(globalXposition < x){
            // move left
            xoffset = x - globalXposition;
            motorXturn(LEFT);
            xdirection = LEFT;
        }
        else if(globalXposition > x){
            if(!(PINC & 1<<IN_X_AXIS_LIMIT)){
                // We have reached the x-limit
                globalXposition = 0;
                globalXpulses = 0;
                xoffset = 0;
                motorXoff();
                xdirection = NONE;
            }
            else {
                // Move right
                xoffset = globalXposition - x;
                motorXturn(RIGHT);
                xdirection = RIGHT;
            }
        }
        else if(globalXposition == x){
            // turn X motor off
            xoffset = 0;
            motorXoff();
        }
        
        /* Y-axis */
        if(globalYposition < y){
            // move up
            yoffset = y - globalYposition;
            motorYturn(UP);
            ydirection = UP;
        }
        else if(globalYposition > y){
            if(!(PIND & 1<<IN_Y_AXIS_LIMIT)){
                globalYposition = 0;
                globalYpulses = 0;
                globalYpulsesActual = 0;
                yoffset = 0;
                motorYoff();
                xdirection = NONE;
            }
            else {
                // Move down; Check for IN_Y_AXIS_LIMIT
                yoffset = globalYposition - y;
                motorYturn(DOWN);
                ydirection = DOWN;
            }
        }
        else if(globalYposition == y){
            // turn Y motor off
            yoffset = 0;
            motorYoff();
        }
        
        if(xoffset!=0){
            xoffset = xoffset - globalXpulses;
        }
        if(yoffset!=0){
            yoffset = yoffset - globalYpulses;
        }
  
        // FIXME!!!! 
        // TODO update globalX  position !!!
        if(xdirection == LEFT){
            globalXposition = globalXposition + globalXpulses;
        }
        else if(xdirection == RIGHT){
            globalXposition = globalXposition - globalXpulses;
        }
        
        // TODO update globalY position !!!
        if(ydirection == UP){
            globalYposition = globalYposition + globalYpulses;
        }
        else if(xdirection == DOWN){
            globalYposition = globalYposition - globalYpulses;
        }
        
        // If both offsets are zero we have reach our position
        if ((xoffset == 0) && (yoffset == 0)){
            positionreached =1;
            globalXposition = x;
            globalYposition = y;
            motorXoff();
            motorYoff();
        }
    }
}


void motorXturnSteps(uint8_t direction, uint8_t steps){
    globalXpulses= 0;
    do {
        motorXturn(direction);
    }
    while (globalXpulses < steps);
    motorXoff();
}

void motorYturnSteps(uint8_t direction, uint8_t steps){
    globalYpulses= 0;
    globalYpulsesActual = 0;
    do {
        motorYturn(direction);
    }
    while (globalYpulses < steps);
    motorYoff();
}

void motorXmoveToPosition(uint8_t position)
{
    if (globalXposition < position){
        motorXturnSteps(LEFT, position - globalXposition);
    }
    else if (globalXposition > position){
        motorXturnSteps(RIGHT, globalXposition - position);
    }
    else {
        // do nothing i.e. position was already reached
    }
    globalXposition = position;
}


void motorYmoveToPosition(uint8_t position)
{
    if (globalYposition < position){
        motorYturnSteps(UP, position - globalYposition);
    }
    else if (globalYposition > position){
        motorYturnSteps(DOWN, globalYposition - position);
    }
    else {
        // do nothing i.e. position was already reached
    }
    globalYposition = position;
}


uint8_t joystickState(void)
{
    if(  ((PINB & 1<<IN_JOYSTICK_LEFT) ? 1: 0)   \
       && ((PINC & 1<<IN_JOYSTICK_RIGHT) ? 1: 0) \
       && ((PINC & 1<<IN_JOYSTICK_UP) ? 1: 0)    \
       && ((PINB & 1<<IN_JOYSTICK_DOWN) ? 1: 0) ){
        return JOYSTICK_STATE_CENTRE;
    }
    else if ( (PINB & (1<<IN_JOYSTICK_LEFT)) ? 0: 1 ) {
        return JOYSTICK_STATE_LEFT;
    }
    else if((PINC & (1<<IN_JOYSTICK_RIGHT)) ? 0: 1) {
        return JOYSTICK_STATE_RIGHT;
    }
    else if((PINC & (1<<IN_JOYSTICK_UP)) ? 0: 1) {
        return JOYSTICK_STATE_UP;
    }
    else if((PINB & (1<<IN_JOYSTICK_DOWN)) ? 0: 1) {
        return JOYSTICK_STATE_DOWN;
    }
    return JOYSTICK_STATE_CENTRE;
}


void motorXturn(uint8_t direction)
{
    if(direction == LEFT) {
        PORTD &= ~(1<<OUT_MOTOR_X2);
        PORTD |= (1<<OUT_MOTOR_X1);
    }
    else {  // Right
        PORTD &= ~(1<<OUT_MOTOR_X1);
        PORTD |= (1<<OUT_MOTOR_X2);
    }
}



void motorYturn(uint8_t direction)
{
    if(direction == UP) {
        PORTB &= ~(1<<OUT_MOTOR_Y2);
        PORTD |= (1<<OUT_MOTOR_Y1);
    }
    else {  // DOWN
        PORTD &= ~(1<<OUT_MOTOR_Y1);
        PORTB |= (1<<OUT_MOTOR_Y2);
    }
}


void motorZturn(uint8_t direction)
{
    if(direction == IN) {
        PORTB &= ~(1<<OUT_MOTOR_Z2);
        PORTB |= (1<<OUT_MOTOR_Z1);
    }
    else {  // OUT
        PORTB &= ~(1<<OUT_MOTOR_Z1);
        PORTB |= (1<<OUT_MOTOR_Z2);
    }
}

void motorXoff(void)
{
    PORTD &= ~((1<<OUT_MOTOR_X1) | (1<<OUT_MOTOR_X2));
}

void motorYoff(void)
{
    PORTD &= ~(1<<OUT_MOTOR_Y1);
    PORTB &= ~(1<<OUT_MOTOR_Y2);
}

void motorZoff(void)
{
    PORTB &= ~((1<<OUT_MOTOR_Z1) | (1<<OUT_MOTOR_Z2));
}


void ledOn(void)
{
    //PORTD |= (1<<OUT_LED);
    PORTD &= ~(1<<OUT_LED);
}

void ledOff(void)
{
    PORTD |= (1<<OUT_LED);
    //PORTD &= ~(1<<OUT_LED);
}

void ledToggle(void)
{
    PORTD ^= (1<<OUT_LED);
}


/**
 * Potmeter ADC conversion
 */
uint16_t readPotMeter(void)
{
    uint16_t adc_value;
   
    ADCSRA |= (1<<ADSC); // start conversion
    while((ADCSRA & (1<<ADSC))==1<<ADSC) {
        // wait for conversion to complete.
    }
    adc_value = ADCL; // first read ADCL then ADCH; read datasheet!
    adc_value |= ADCH<<8 ;
    return adc_value;
}

/* EOF */

