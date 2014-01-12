#ifndef _FISHERTECNIKMAGAZIJNMODEL_H
#define _FISHERTECHNIKMAGAZIJNMODEL_H
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


/* OUTPUTS */
#define OUT_MOTOR_X1 PD3
#define OUT_MOTOR_X2 PD5
#define OUT_MOTOR_Y1 PD6
#define OUT_MOTOR_Y2 PB3
#define OUT_MOTOR_Z1 PB2
#define OUT_MOTOR_Z2 PB1
#define OUT_LED PD3

/* INPUTS */
#define IN_POTMETER PC0
#define IN_JOYSTICK_LEFT PB4
#define IN_JOYSTICK_RIGHT PC2
#define IN_JOYSTICK_UP PC1
#define IN_JOYSTICK_DOWN PB5


/* Program Constants */
// For the Motors
#define LEFT 0
#define RIGHT 1
#define UP 0
#define DOWN 1
#define IN 0
#define OUT 1

// For the Joystick INPUT state
#define JOYSTICK_STATE_CENTRE 0x01
#define JOYSTICK_STATE_LEFT 0x02
#define JOYSTICK_STATE_RIGHT 0x03
#define JOYSTICK_STATE_UP 0x04
#define JOYSTICK_STATE_DOWN 0x05



/**
 * Prototypes:
 */
void flashntimes(int n);
void allOff(void);
void motorXturn(int direction);
void motorYturn(int direction);
void motorZturn(int direction);
void motorXoff(void);
void motorYoff(void);
void motorZoff(void);
void motorTurnSteps(int direction, int steps);
uint8_t joystickState(void);
void ledOn(void);
void ledOff(void);
uint16_t readPotMeter(void);

/* EOF */
#endif