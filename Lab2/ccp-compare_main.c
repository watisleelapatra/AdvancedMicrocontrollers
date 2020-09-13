#include "mcc_generated_files/mcc.h"


void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    TRISCbits.TRISC2=0;		/* Configure RC2 pin as output. */
    CCP1CON=9;		/* Compare mode, when CCPR1 and TMR3 match, RC2 is driven high*/
    PIR1bits.CCP1IF=0;
    TMR3=0;
    T3CON=0xC0;		/* Timer3 used for capture mode with TMR3 register in 16-bit format*/
    CCPR1=0x320;		/* Load a count for generating 0.4ms*/
    T3CONbits.TMR3ON=1;	/* Turn On Timer3*/   
    while(1)    {
       /* Wait for CCP Interrupt flag to set, it is set on compare match between CCPR1 and TMR3*/
        while(PIR1bits.CCP1IF==0);
        PIR1bits.CCP1IF=0;		/* Clear interrupt flag*/          
        TMR3=0;		/* Clear Timer3 Register*/
        CCP1CON=8;		/* Compare mode, when CCPR1 and TMR3 match, RC2 is driven low*/
        CCPR1=0x04B0;		/* Load count for generating 0.6ms */
        while(PIR1bits.CCP1IF==0);
        PIR1bits.CCP1IF=0;
        TMR3=0;		/* Clear Timer3 Register*/
        CCP1CON=9;
        CCPR1=0x0320;		/* Load count for generating 0.4ms */
    }
}
