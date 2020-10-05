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

#include <xc.h>
#define _XTAL_FREQ 1000000  //Use internal OSC, Freq = 1MHz (default value)
#define CS LATCbits.LATC0   //Slave select signal

void SPI_Init_Master(void);
void SPI_Write(unsigned char x);

void main(void) 
{
    unsigned char d = 0;
    
    SPI_Init_Master();
    CS = 0;     //slave select
    SPI_Write(0x11);    //Command byte xx01xx01, write data to pot0 
    SPI_Write(0x00);    //Set wiper to lowest value, Dn = 0
    CS = 1;     //
    while(1) {    
        CS = 0;
        SPI_Write(0x11);
        SPI_Write(d);
        CS = 1;
        d += 10;
        __delay_ms(2000);
    }
}

void SPI_Init_Master(void)
{
    /* PORT definition for SPI pins*/    
    TRISCbits.RC4=1;        /* RC4 as input(SDI) */
    TRISCbits.RC3=0;		/* RC3 as output(SCK) */
    TRISCbits.RC5=0;		/* RC5 as output(SDO) */
    TRISCbits.RC0=0;        //CS
    /* To initialize SPI Communication configure following Register*/
    CS = 1;
    SSPSTAT=0x40;		/* Data change on rising edge of clk , BF=0*/
    SSPCON1=0x22;		/* Master mode,Serial enable, idle state low for clk, fosc/64 */ 
    PIR1bits.SSPIF=0;

    /* Disable the ADC channel which are on for multiplexed pin when used as an input */    
    ADCON0=0;			/* This is for de-multiplexed the SCL and SDI from analog pins*/
    ADCON1=0x0F;		/* This makes all pins as digital I/O */
}

void SPI_Write(unsigned char x)
{
    unsigned char data_flush;
    SSPBUF=x;			/* Copy data in SSBUF to transmit */
    while(!PIR1bits.SSPIF);	/* Wait for complete 1 byte transmission */
    PIR1bits.SSPIF=0;		/* Clear SSPIF flag */
    data_flush=SSPBUF;		/* Flush the data */
}