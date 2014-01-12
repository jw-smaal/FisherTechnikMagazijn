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
void flashntimes(int n){
    int i = 0;
    for (i = 0; i < n; i++){
        ledOn();
        _delay_ms(600);
        ledOff();
        _delay_ms(400);
    }
}

void allOff()
{
    ledOff();
    motorXoff();
    motorYoff();
    motorZoff();
}


void motorTurnSteps(int direction, int steps){
    //motorXturn(direction);
    //motorCountSteps(steps);
    //motorOff();
}

void motorXturn(int direction)
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


void motorYturn(int direction)
{
    if(direction == LEFT) {
        PORTB &= ~(1<<OUT_MOTOR_Y2);
        PORTD |= (1<<OUT_MOTOR_Y1);
    }
    else {  // Right
        PORTD &= ~(1<<OUT_MOTOR_Y1);
        PORTB |= (1<<OUT_MOTOR_Y2);
    }
}


void motorZturn(int direction)
{
    if(direction == LEFT) {
        PORTB &= ~(1<<OUT_MOTOR_Z2);
        PORTB |= (1<<OUT_MOTOR_Z1);
    }
    else {  // Right
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
   // PORTD |= (1<<OUT_LED);
}

void ledOff(void)
{
    //PORTD &= ~(1<<OUT_LED);
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

