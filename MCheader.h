/**************************************************
EE318 irragation project
Module name                     : MCheader.h
Date of last modification       : 19/04/2016
last update                     : deletion of the unused timer initilization function
Author                          : Rowen Simpson
 **************************************************/

#include <msp430.h>
#include <driverlib.h>

extern void ADC_initilize();  // initilize ADC registers
extern void LED_initilize();  // initilize LED pins
extern void UART_initilize(); // initilize UART