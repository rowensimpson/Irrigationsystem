/**************************************************
EE318 irragation project
Module name                     : MCheader.c 
Date of last modification       : 19/04/2016
last update                     : deletion of the unused timer interrupt
Author                          : Rowen Simpson
 **************************************************/

#include "MCheader.h"

void ADC_initilize()
{
    ADCCTL0 |= ADCSHT_2 + ADCON + ADCMSC;                       // controls interval of sampling timer and switches adc channel on
    ADCCTL1 |= ADCSHP + ADCDIV_4 + ADCSSEL_2 + ADCCONSEQ_3;     //ADC clock divider(ADCDIV_4), clk selection(ADCSSEL), pulse sample mode(ADCSHP)
    ADCCTL2 |= ADCRES;                                          //set the resolution of digital signal     
    ADCMCTL0 |= ADCINCH_15 + ADCSHT_0;                          // set reference voltages and select input channel
    ADCIE = ADCIE0;                                             //enable the interrupt 
    ADCIFG = ADCIFG0;                                           // clear interrupt
    SYSCFG2|=ADCPCTL9+ADCPCTL8;                                 // set A8 and A9 as inputs
}

void LED_initilize()
{
    //configure pins
    P1OUT &= 0x00; // clear all pins
    P1DIR &= 0x00;
    P2OUT &= 0x00; // clear all pins
    P2DIR &= 0x00;
    P4OUT &= 0x00; // clear all pins
    P4DIR &= 0x00;
    //set output pins 
    P1DIR |= 0x78; // set required port 1 pins as outputs 
    P2DIR |= 0xA0; // set required port 2 pins as outputs
    P4DIR |= 0x01; // set required port 4 pins as outputs
       
    //power indicator led on
    P4OUT |= 0x01;
}
void UART_initilize() // UART initilization 
{
    P1SEL0 = 0x03;
    UCA0CTL1 |= UCSWRST;
    UCA0CTL1 |= UCSSEL_2;
    UCA0BR0 = 9;
    UCA0BR1 = 0;
    UCA0MCTLW |= UCBRS1 + UCBRF0;
    UCA0CTL1 &= ~UCSWRST;
    UCA0IE |= UCRXIE;
}