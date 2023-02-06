/*
 * OccupancyDetector.c
 *
 *  Created on: Jan 31, 2023
 *      Author: Craig Droke
 *
 * The goal of this project is to create a detector that blinks green every three seconds when the sensor is not active (indicating no occupancy).
 * When the sensor value turns to a 1, the state changes to blink red (indicating the opposite). After 10 seconds it becomes a
 * constant red flash for another 5 seconds untill the only way to return to the armed state is through button reset.
 */



#include <msp430.h>

int main(){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // Configure GPIO
        P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state RED
        P1DIR |= BIT0;                          // Set P1.0 to output direction

        P6OUT &= ~BIT6;                         // Clear P6.6 output latch for a defined power-on state GREEN
        P6DIR |= BIT6;                          // Set P6.6 to output direction

        P2DIR &= ~BIT3;                         //Set P2.1 as input (sensor)
        P2REN |= BIT3;               // Enable Resistor on P2.3
        P2OUT |= BIT3;               // Configure Resistor on P2.3 to be Pullup

        //Implement input pin from sensor

        P4REN |= BIT1;               // Enable Resistor on P4.1
        P4OUT |= BIT1;               // Configure Resistor on P4.1 to be Pullup

        //P4REN |= BIT1;                          // P4.1 pull-up register enable
       // P4IES &= ~BIT1;                         // P4.1 Low --> High edge
        //P4IE |= BIT1;                           // P4.1 interrupt enabled

        int count = 0;                          //counts time for 10 seconds
        int totalert = 0;                       //Flag to stop from being able to go into armed state after 15 seconds
        int alert = 0;                          //Flag to differentiate >10seconds in warning

     PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode

        while(1)
        {

          if ((P2IN & BIT3) && totalert == 0){  //pin that reads in sensor is 0 and totalert is 0
              P1OUT &= ~BIT0;
              P6OUT ^= BIT6;    //Blink
              __delay_cycles(500000);             // Delay for 100000*(1/MCLK)=.5s
              P6OUT &= ~BIT6;
              count = 0;
              alert = 0;
              __delay_cycles(2500000);             // Delay for 2500000*(1/MCLK)=2.5s
          }
          if (!(P2IN & BIT3) && alert == 0) //Pin for sensor is a 1 and alert is 0
          {
              P1OUT ^= BIT0;
              __delay_cycles(500000);             // Delay for 500000*(1/MCLK)=0.5s
              P1OUT &= ~BIT0;
              __delay_cycles(500000);             // Delay for 500000*(1/MCLK)=0.5s
              count++;

          }
          if (count >= 10){  // sensor is still a 1 after 10 seconds
             alert = 1;
             P1OUT |= BIT0;         //Turn on red constantly indicating alert state (THIS ISNT WORKING)
             count++;
             __delay_cycles(1000000);             // Delay for 100000*(1/MCLK)=1s

             if (count >= 15) { //greater than or= to 15 seconds
                totalert = 1; //perm locks into state until button pressed

                }

          }

          if (!(P4IN & BIT1)) { //Button pressed
              totalert = 0;                //go back to armed state regardless of where at: set totalert back to 0;
          }

        }

    return 0;
}




