/** C O N F I G U R A T I O N   B I T S ******************************/
#pragma config FOSC = INTIO67, FCMEN = OFF, IESO = OFF                      // CONFIG1H
#pragma config PWRT = OFF, BOREN = OFF, BORV = 30                           // CONFIG2L
#pragma config WDTEN = OFF, WDTPS = 32768                                   // CONFIG2H
#pragma config MCLRE = ON, LPT1OSC = OFF, PBADEN = ON, CCP2MX = PORTC       // CONFIG3H
#pragma config STVREN = ON, LVP = OFF, XINST = OFF                          // CONFIG4L
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF                   // CONFIG5L
#pragma config CPB = OFF, CPD = OFF                                         // CONFIG5H
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF               // CONFIG6L
#pragma config WRTB = OFF, WRTC = OFF, WRTD = OFF                           // CONFIG6H
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF           // CONFIG7L
#pragma config EBTRB = OFF                                                  // CONFIG7H

#define _XTAL_FREQ      1000000     //default value
#define Switch_Pin      PORTBbits.RB0
#define DetectsInARow   5

#include <xc.h>

typedef enum {LEFT2RIGHT, RIGHT2LEFT} LEDDirections;
typedef enum {FALSE, TRUE} BOOL;
unsigned char LED_Display;  // 8-bit variable

void main (void)
{
    LEDDirections Direction = LEFT2RIGHT;
    BOOL SwitchPressed = FALSE;

    LED_Display = 1;            // initialize

    // Init I/O
    TRISD = 0b00000000;     	// PORTD bits 7:0 are all outputs (0)

	INTCON2bits.RBPU = 0;		// enable PORTB internal pullups
	WPUBbits.WPUB0 = 1;			// enable pull up on RB0
    ANSELH = 0x00;              // AN8-12 are digital inputs (AN12 on RB0)

    TRISBbits.TRISB0 = 1;       // PORTB bit 0 (connected to switch) is input (1)

    // Init Timer
    INTCONbits.TMR0IF = 0;          // clear roll-over interrupt flag
    T0CON = 0b00001000;             // no prescale - increments every instruction clock
    //T0CON = 0b00000001;             // prescale 1:4 - four times the delay.
    TMR0H = 0;                      // clear timer - always write upper byte first
    TMR0L = 0;
    T0CONbits.TMR0ON = 1;           // start timer

    while (1) {
        if (Direction == LEFT2RIGHT) {
            LED_Display <<= 1;          // rotate display by 1 from 0 to 7
            if (LED_Display == 0)
                LED_Display = 1;        // rotated bit out, so set bit 0
        }
        if (Direction == RIGHT2LEFT) {
            LED_Display >>= 1;          // rotate display by 1 from 7 to 0
            if (LED_Display == 0)
                LED_Display = 0x80;     // rotated bit out, so set bit 7
        }
        LATD = LED_Display;         // output LED_Display value to PORTD LEDs
        do { // poll the switch while waiting for the timer to roll over.
            if (Switch_Pin == 1) { // look for switch released.
                SwitchPressed = FALSE;
            }
            else if (SwitchPressed == FALSE) { // switch was just pressed
                SwitchPressed = TRUE;
                // change  direction
                if (Direction == LEFT2RIGHT)
                    Direction = RIGHT2LEFT;
                else
                    Direction = LEFT2RIGHT;
            }
        } while (INTCONbits.TMR0IF == 0);
        // Timer expired
        INTCONbits.TMR0IF = 0;          // Reset Timer flag
    }
}
