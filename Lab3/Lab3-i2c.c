/*
 * File:   f4550-i2c.c
 * Author: User
 *
 * Created on October 3, 2020, 10:13 PM
 */
#define _XTAL_FREQ 1000000      //Default INTOSC Freq
#define WriteADDR 0x70
#define ReadADDR 0x71

// CONFIG1L
#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = INTOSC_XT // Oscillator Selection bits (Internal oscillator, XT used by USB (INTXT))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = ON         // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

void I2C_Init(unsigned long clock_freq);
char I2C_Start(char slave_write_address);
void I2C_Ready();
char I2C_Write(unsigned char data);
char I2C_Stop();
char I2C_Read(char flag);
void I2C_Ack();
void I2C_Nack();

#include <xc.h>

void main(void) {
    unsigned char i, dat;
    
    I2C_Init(100000);
    I2C_Start(0x70);        //Send Start condition to device Write Addr 70h
    for(i=1; i<5; i++) {    //Blink LED 5 times
        I2C_Write(0x00);    //LEDs ON (Active low)
        __delay_ms(500);
        I2C_Write(0xFF);    //LEDs OFF    
        __delay_ms(500);
    }    
    TRISCbits.TRISC0 = 0;   //Output LED
    LATCbits.LATC0 = 0;     //Off LED
    
    I2C_Start(0x71);        //Send Start condition to device Read Addr 71h
    while(1) {
        dat = I2C_Read(0);    //Continuous reading, flag = 0
        LATCbits.LATC0 = dat >> 2;
    }
}
void I2C_Init(unsigned long clock_freq) {
    TRISB0=1;		/* Set up I2C lines by setting as input */
    TRISB1=1;
    SSPSTAT=0x80;		/* Slew rate disabled, other bits are cleared */
    SSPCON1=0x28;	/* Enable SSP port for I2C Master mode,	clock = FOSC / (4 * (SSPADD+1))*/ 
    SSPCON2=0;
    SSPADD=(_XTAL_FREQ/(4*clock_freq))-1;	/* Clock 100 kHz */  
    SSPIE=1;		/* Enable SSPIF interrupt */
    SSPIF=0;
}
char I2C_Start(char slave_write_address) {   
    SSPCON2bits.SEN=1;		/* Send start pulse */
    while(SSPCON2bits.SEN);	/* Wait for completion of start pulse */
    SSPIF=0;
    if(!SSPSTATbits.S)		/* Check whether START detected last */
        return 0;			/* Return 0 to indicate start failed */   
    return (I2C_Write(slave_write_address));	/* Write slave device address with write to communicate */
}
void I2C_Ready() {
    while(!SSPIF);	/* Wait for operation complete */
    SSPIF=0;		/* Clear SSPIF interrupt flag*/
}
char I2C_Write(unsigned char data) {
      SSPBUF=data;	/* Write data to SSPBUF*/
      I2C_Ready();
      if (ACKSTAT)	/* Check for acknowledge bit*/
        return 1;
      else
        return 2;
}
char I2C_Stop() {
    I2C_Ready();
    PEN=1;		/* Stop communication*/
    while(PEN);		/* Wait for end of stop pulse*/
    SSPIF = 0;
    if (!SSPSTATbits.P) /* Check whether STOP is detected last */
       return 0;	/* If not return 0 to indicate start failed*/
    return 1;
}
char I2C_Read(char flag) {
        int buffer=0;
        RCEN=1;			/* Enable receive */

	/* Wait for buffer full flag which when complete byte received */
        while(!SSPSTATbits.BF);
        buffer=SSPBUF;		/* Copy SSPBUF to buffer */

	/* Send acknowledgment or negative acknowledgment after read to continue or stop reading */
        if(flag==0)
            I2C_Ack();
        else
            I2C_Nack();
        I2C_Ready();
        return(buffer);
}
void I2C_Ack() {
    ACKDT=0;		/* Acknowledge data 1:NACK,0:ACK */
    ACKEN=1;		/* Enable ACK to send */
    while(ACKEN);
 }
void I2C_Nack() {
    ACKDT=1;		/* Acknowledge data 1:NACK,0:ACK */
    ACKEN=1;		/* Enable ACK to send */
    while(ACKEN);
}