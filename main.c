/**************************************************
EE318 irragation project
Module name                     : main.c 
Date of last modification       : 19/04/2016
last update                     : Control limit update states
Author                          : Rowen Simpson
 **************************************************/

#include <msp430.h>
#include <driverlib.h>
#include "MCheader.h"                                   //used for the initilizarion of the ADC,TIMER and PORTS 
#include "hal_LCD.h"                                    // used to provide functions for the LCD display 

int ADC_value[16] = {0};                                // ADC value storage array 
int ADC_Channel = 0;                                    // ADC channel index
int mos1,mos2,mos1t,mos2t,tp=0,i;                       // moisture limit and level variables
char a[5];                                              // UART collection array  
int temp = 0;                                           // temperature variable

  #pragma vector=ADC_VECTOR                             // ADC interrupt service routine used to assign ADC values to ADC array.     
  __interrupt void ADC_ISR(void)
  {
    switch(__even_in_range(ADCIV,ADCIV_ADCIFG))
    {
      case ADCIV_ADCIFG:                                // conversion complete
        
        ADC_value[ADC_Channel] = ADCMEM0;               // save ADC value to corresponding array element {ADC_value[ADC3,ADC2,ADC1]}     
        ADC_Channel = ADCMCTL0 & (ADCINCH0 + ADCINCH1 + ADCINCH2 + ADCINCH3); // used to read the value of ADC channel currently converting 
      break;
    }
  }
          

 int main (void)
 {WDTCTL = WDTPW | WDTHOLD;                             // stop watchdog 
  
    ADC_initilize();                                    // initilize the adc channels that are to be monitored
    
    LED_initilize();                                    // clear and initilize led outputs
    
    UART_initilize();
    
    __enable_interrupt();                               // enter low power mode
    
    PMM_unlockLPM5();                                   // power management function used to reset LPMx.5 and thus unlock pins.
    
    ADCCTL0 |= ADCENC + ADCSC;                          //ADC sampling start
    
    Init_LCD();                                         // initilize the LCD 
    clearLCD();                                         // reset the LCD display
    PMM_enableTempSensor();                             // enable the temperature sensor on array element 12 

     int b[4];
  
    while (1) 
     {
        temp = 0.00355 * (ADC_value[12] - 326) + 25;    // temperature voltage to celcius conversion
        mos1=0.16*ADC_value[8];                    // moisture voltage to moisture percentsge conversion
        mos2=0.16*ADC_value[9];                    // moisture voltage to moisture percentsge conversion
       
        // LED positioning
        b[0]=temp/1000;
        b[1]=temp/100%10;
        b[2]=temp/10%10;
        b[3]=temp%10;
        showChar(b[0]+48,pos3);
        showChar(b[1]+48,pos4);
        showChar(b[2]+48,pos5);
        showChar(b[3]+48,pos6);

        // Output pin list
        // Channel 9 Valve pin : P1.4[00010000]
        // Channel 8 Valve pin : P1.3[00001000]
        // Channel 9 LED pins  : P1.5(Valve active)[00100000], P1.6(Valve non-active)[01000000] 
        // Channel 8 LED pins  : P2.5(Valve active)[00100000], P2.7(Valve non-active)[10000000]
        
        if (mos1 < mos1t && mos2 < mos2t)                   // channel 8 and 9 are below the desired moisture value  
        {
          P1OUT = 0x38; // 00111000 
          P2OUT = 0x20; // 00100000
        }
        else if (mos2 < mos2t && mos1 >= mos1t)             //channel 9 is below the desire moisture value
        {
          P1OUT = 0x30; // 00110000
          P2OUT = 0x80; // 10000000   
        }
        else if (mos2 >= mos2t && mos1 < mos1t)             // channel 8 is below the desired moisture level
        {
          P1OUT = 0x48; //01001000
          P2OUT = 0x20; //00100000
        }
        else if (mos2 >= mos2t && mos1 >= mos1t)            // no channels are below the desired moisture level
        {
          P1OUT = 0x40; //01000000
          P2OUT = 0x80; //10000000
        }
        else
        {                                                   // error state, in error state all LEDS will flash. details errors in mos1t and mos2t settings. useful for debugging 
          P2OUT ^= 0xA0;
          P1OUT ^= 0x60;
        __delay_cycles(100000);                             // provides delay between LED on/off times        
        }

}
}

#pragma vector=USCI_A0_VECTOR                           //UART interrupt 
__interrupt void USCI_A0_ISR(void)
{
  switch(__even_in_range(UCA0IV,4))
  {
  case 0:break;                                         // Vector 0 - no interrupt
  case 2:                                               // Vector 2 - RXIFG
                                                        // USCI_A0 TX buffer ready?
    //UCA0TXBUF = UCA0RXBUF;                            // TX -> RXed character
    
    a[i]=UCA0RXBUF;                                     // recieve buffer 
    i++;
    if (i==5)
    {
      i=0;
      if (a[0]=='t'&&a[3]=='f')                         // test for moisture setting information
      {mos1t=a[1];mos2t=a[2];}                          // sets the moisture setting. recieved from computer
      if (a[0]=='s'&&a[1]=='s'&&a[2]=='s'&&a[3]=='s'&&a[4]=='s') // tests for sending information
      {
      UCA0TXBUF=115;                                    // preamble 
      while (!(UCA0IFG&UCTXIFG));
      UCA0TXBUF=mos1;                                   // sends moisture level of mosture sensor 1
      while (!(UCA0IFG&UCTXIFG));
      UCA0TXBUF=mos2;                                   // sends moisture level of sensor 2
      while (!(UCA0IFG&UCTXIFG));
      UCA0TXBUF=temp;                                   // sends temperature level
      while (!(UCA0IFG&UCTXIFG));
      UCA0TXBUF=101;                                    // closes the packet
      while (!(UCA0IFG&UCTXIFG));
      }
    }
    break;
  case 4:break;                                         // Vector 4 - TXIFG
  default: break;
  }
}
