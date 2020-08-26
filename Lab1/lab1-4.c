/** C O N F I G U R A T I O N   B I T S ******************************/
#pragma config FOSC = INTIO67, FCMEN = OFF, IESO = OFF                       // CONFIG1H
#pragma config PWRT = OFF, BOREN = OFF, BORV = 30                        // CONFIG2L
#pragma config WDTEN = OFF, WDTPS = 32768                                     // CONFIG2H
#pragma config MCLRE = ON, LPT1OSC = OFF, PBADEN = ON, CCP2MX = PORTC       // CONFIG3H
#pragma config STVREN = ON, LVP = OFF, XINST = OFF                          // CONFIG4L
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF                   // CONFIG5L
#pragma config CPB = OFF, CPD = OFF                                         // CONFIG5H
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF               // CONFIG6L
#pragma config WRTB = OFF, WRTC = OFF, WRTD = OFF                           // CONFIG6H
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF           // CONFIG7L
#pragma config EBTRB = OFF                                                  // CONFIG7H

#define _XTAL_FREQ      1000000
#define Switch_Pin      PORTBbits.RB0
#define DetectsInARow   5

#include <xc.h>

typedef enum {LEFT2RIGHT, RIGHT2LEFT} LEDDirections;
typedef enum {FALSE, TRUE} BOOL;
unsigned char LED_Display;  // 8-bit variable

void Timer0_Init(void);
void ADC_Init(void);
unsigned char ADC_Convert(void);

void main (void)
{
    LEDDirections Direction = LEFT2RIGHT;
    BOOL SwitchPressed = FALSE;

    LED_Display = 1;            // initialize
    // Init I/O
    TRISD = 0b00000000;     	// PORTD bits 7:0 are all outputs (0)
	TRISAbits.TRISA0 = 1;		// TRISA0 input

	INTCON2bits.RBPU = 0;		// enable PORTB internal pullups
	WPUBbits.WPUB0 = 1;			// enable pull up on RB0

    // ADCON1 is now set up in the InitADC() function.
    TRISBbits.TRISB0 = 1;       // PORTB bit 0 (connected to switch) is input (1)

    // Init Timer0
    Timer0_Init();

    // Init ADC
    ADC_Init();

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
        // Take an ADC conversion and use it to set Timer0
        TMR0H = ADC_Convert();      // MSB from ADC
        TMR0L = 0;                  // LSB = 0
    }
}

void Timer0_Init(void)
{
    INTCONbits.TMR0IF = 0;          // clear roll-over interrupt flag
    T0CON = 0b00000001;             // prescale 1:4 - about 1 second maximum delay.
    TMR0H = 0;                      // clear timer - always write upper byte first
    TMR0L = 0;
    T0CONbits.TMR0ON = 1;           // start timer
}

void ADC_Init(void)
{   // initialize the Analog-To-Digital converter.
	ANSEL = 0;	//turn off all other analog inputs
	ANSELH = 0;
 	ANSELbits.ANS0 = 1;	// turn on RA0 analog
    ADCON1 = 0;
    // ADFM = 0 so we can easily read the 8 Most Significant bits from the ADRESH
    // Special Function Register
    ADCON2 = 0b00111000;
    // Select channel 0 (AN0) to read the potentiometer voltage and turn on ADC
    ADCON0 = 0b00000001;
}

unsigned char ADC_Convert(void)
{ // start an ADC conversion and return the 8 most-significant bits of the result
    ADCON0bits.GO_DONE = 1;             // start conversion
    while (ADCON0bits.GO_DONE == 1);    // wait for it to complete
    return ADRESH;                      // return high byte of result
}
