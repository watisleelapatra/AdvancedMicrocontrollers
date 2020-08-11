/** C O N F I G U R A T I O N   B I T S ******************************/
#pragma config FOSC = INTIO67, FCMEN = OFF, IESO = OFF                       // CONFIG1H
#pragma config PWRT = OFF, BOREN = SBORDIS, BORV = 30                        // CONFIG2L
#pragma config WDTEN = OFF, WDTPS = 32768                                     // CONFIG2H
#pragma config MCLRE = OFF, LPT1OSC = OFF, PBADEN = ON, CCP2MX = PORTC       // CONFIG3H
#pragma config STVREN = ON, LVP = OFF, XINST = OFF                          // CONFIG4L
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF                   // CONFIG5L
#pragma config CPB = OFF, CPD = OFF                                         // CONFIG5H
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF               // CONFIG6L
#pragma config WRTB = OFF, WRTC = OFF, WRTD = OFF                           // CONFIG6H
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF           // CONFIG7L
#pragma config EBTRB = OFF                                                  // CONFIG7H

#define _XTAL_FREQ 1000000  //Use internal OSC, Freq = 1MHz (default value)
#include <xc.h>

unsigned char LED_Number;  // 8-bit variable
unsigned char LED_LookupTable[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

void main (void)
{
    LED_Number = 0;            // initialize
    TRISD = 0b00000000;     	// PORTD bits 7:0 are all outputs (0)
    while (1)
    {
		// use lookup table to output one LED on based on LED_Number value
        LATD = LED_LookupTable[LED_Number];    
        LED_Number++;      // rotate display by 1
        if (LED_Number == 8)
            LED_Number = 0;    // go back to LED 0.
        __delay_ms(500);	    // Delay 50 x 1000 = 50,000 cycles; 200ms @ 1MHz
    }	
}