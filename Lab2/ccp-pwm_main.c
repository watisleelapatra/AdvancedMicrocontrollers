#include "mcc_generated_files/mcc.h"
#define X_FREQ 8000000

void main(void)
{
    unsigned char brightness = 125; // = 0x7D
    // Initialize the device
    SYSTEM_Initialize();
    // Set RD7/P1D pin output so P1D PWM output drives LED7
    TRISDbits.TRISD7 = 0;

    // Set up 8-bit Timer2 to generate the PWM period (frequency)
    T2CON = 0b00000111;// Prescale = 1:16, timer on, postscale not used with CCP module
    PR2 = 249;         // Timer 2 Period Register = 250 counts
    // Thus, the PWM frequency is:
    // 1MHz clock / 4 = 250kHz instruction rate.
    // (250kHz / 16 prescale) / 250) = 62.5Hz, a period of 16ms.

    // The Duty Cycle is controlled by the ten-bit CCPR1L<7,0>:DC1B1:DC1B0
    // 50% Duty Cycle = 0.5 * (250 * 4) = 500
    CCPR1L = 0x7D;   // The 8 most significant bits of the value 500 = 0x1F4 are 0x7D
                     // The 2 least significant bits of the value (0b00) are written
                     // to the DC1B1 and DC1B0 bits in CCP1CON
    CCP1CON = 0b01001100;
                     // P1Mx = 01 Full-Bridge output forward, so we get the PWM
                     // signal on P1D to LED7.  Only Single Output (00) is needed,
                     // but the P1A pin does not connect to a demo board LED
                     // CCP1Mx = 1100, PWM mode with P1D active-high.

    // The LED brightness is affected by by the Duty Cycle, which determines how much
    // of each 16ms period it is on and how much it is off.  As the duty cycle gets
    // less than 50%, it is off more than it is on so the LED becomes dimmer.  As the 
    //duty cycle increases above 50%, it is on more than off, so it gets brighter.
    //
    // This increases the brightness over 2 seconds, then decreases it over the next 2 seconds
    // Updating the CCPR1L value more than once per 16ms period has no benefit, so we'll update
    // it a total of 125 times, once per period, which works out to 2 seconds.
    //
    // Although we have nearly ten bits of resolution in the duty cycle (1000 counts)
    // we'll increment the duty cycle by 8 each time as we only have 125 levels over the
    // 2 second period.
    while(1) {
        // increase brightness over 2 seconds.
        do
        {
            brightness += 2;
            CCPR1L = brightness;    // + 8 including 2 bits DC1Bx in CCP1CON
            PIR1bits.TMR2IF = 0;        // clear interrupt flag; set on every TMR2 = PR2 match
            while (PIR1bits.TMR2IF == 0); // watch for match, which is end of period.
        } while (brightness < 250);

        __delay_ms(250);    // delay about 250ms at peak brightness, just for effect!

        // decrease brightness over 2 seconds.
        do {
            brightness -= 2;
            CCPR1L = brightness;    // - 8 including 2 bits DC1Bx in CCP1CON
            PIR1bits.TMR2IF = 0;        // clear interrupt flag; set on every TMR2 = PR2 match
            while (PIR1bits.TMR2IF == 0); // watch for match, which is end of period.
        } while (brightness > 1);

        __delay_ms(250);    // delay about 250ms at dimmest, it gives a better effect!
    };    	
}
