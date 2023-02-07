/*
 * OccupancyDetector.c
 *
 *  Created on: Jan 31, 2023
 *      Author: Craig Droke
 *
 * The goal of this project is to create a detector that blinks green every three seconds when the sensor is not active (indicating no occupancy).
 * When the sensor value turns to a 1, the state changes to blink red (indicating the occupancy). After 10 seconds it becomes a
 * constant red flash (alert mode) for another 5 seconds until the only way to return to the armed state is through button reset.
 *
 * Note: Instead of a sensor, this is being simulated with the use of a button.
 *
 * Note: This design currently uses polling as opposed to interupts.
 */



#include <msp430.h>

int main(){

    WDTCTL = WDTPW | WDTHOLD;                     // Stop watchdog timer

    // Configure GPIO

        P1OUT &= ~BIT0;                           // Clear P1.0 output latch for a defined power-on state RED
        P1DIR |= BIT0;                            // Set P1.0 to output direction

        P6OUT &= ~BIT6;                           // Clear P6.6 output latch for a defined power-on state GREEN
        P6DIR |= BIT6;                            // Set P6.6 to output direction

        P2DIR &= ~BIT3;                           // Set P2.3 to an Input
        P2REN |= BIT3;                            // Enable Resistor on P2.3
        P2OUT |= BIT3;                            // Clear P2.3 button

        P4DIR &= BIT1;                            // Set P4.1 to an Input
        P4REN |= BIT1;                            // Enable Resistor on P4.1
        P4OUT |= BIT1;                            // Clear P4.1 button

    // Instatiate count variables and flags

        int count = 1;                            // Counts time paired with a 1 sec delay
        int totalert = 0;                         // Flag to stop from being able to go into armed (green blink) state after 15 seconds
        int alert = 0;                            // Flag to differentiate > 10 seconds in warning (blink red)

     PM5CTL0 &= ~LOCKLPM5;                        // Disable the GPIO power-on default high-impedance mode

        while(1)
        {

    // This if statement checks if the button P2.3 is NOT pressed (inverted) and that more less than 15 seconds have passed in the warning state.
    // This then blinks green every three seconds and resets the varaibles.

          if ((P2IN & BIT3) && totalert == 0){
              P1OUT &= ~BIT0;                     // Clear P1.0
              P6OUT ^= BIT6;                      // Toggle P6.6
              __delay_cycles(500000);             // Delay for 100000*(1/MCLK)=.5s
              P6OUT &= ~BIT6;                     // Clear P6.6
              count = 1;                          // Reset count
              alert = 0;                          // Reset alert
              __delay_cycles(2500000);            // Delay for 2500000*(1/MCLK)=2.5s
          }

    // This if statement checks if the button P2.3 IS pressed and that it has not been 10 or more seconds.
    // This then blinks red every second (500ms on and off).

          if (!(P2IN & BIT3) && alert == 0){

              P6OUT &= ~BIT6;                     // Clears P6.6
              P1OUT ^= BIT0;                      // Toggle P1.0
              __delay_cycles(500000);             // Delay for 500000*(1/MCLK)=0.5s
              P1OUT &= ~BIT0;                     // Clears P1.0
              __delay_cycles(500000);             // Delay for 500000*(1/MCLK)=0.5s
              count++;                            // Increases count by 1 after the second passes
          }

    // This if statement checks if the count is greater than or equal to 10 seconds.
    // This then sets the alert flag to 1 (preventing access to above if statement) and blinks red perm.

          if (count >= 10){
             alert = 1;                           // Sets the alert flag to 1
             P1OUT |= BIT0;                       // Turns on red led constantly
             count++;                             // Increases count by 1 per second (delay below)
             __delay_cycles(1000000);             // Delay for 100000*(1/MCLK)=1s

       // This nested if statement checks if the count is greater than or equal to 10.
       // This then locks the device into a perm red state requireding reset.

             if (count >= 15) {
                totalert = 1;                     // Perm locks into state until button pressed (totalert flag set to 1)
                }
          }
    // This if statement checks if the button P4.1 is pressed (inverted).
    // This then sets the totalart flag to 0 so that the device can go back to the armed state.

          if (!(P4IN & BIT1)) {
              totalert = 0;                      // Go back to armed state (set totalert back to 0)
          }

        }

    return 0;
}




