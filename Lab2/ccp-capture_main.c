#include "mcc_generated_files/mcc.h"

void main(void)
{
    SYSTEM_Initialize();

    CCP2CON = 0x05;	//Capture mode, on every rising edge
	TRISD = 0;
	TRISCbits.TRISC2 = 1; //CCP1 input pin
	CCPR2H = 0; CCPR2L = 0;
	while(1) {
		TMR1H = 0;
		TMR1L = 0;
		PIR2bits.CCP2IF = 0;	//clear CCP1IF flag
		while(PIR2bits.CCP2IF == 0) //wait for 1st rising edge
		T1CONbits.TMR1ON = 1;	//start TMR1
		PIR2bits.CCP2IF = 0;	//clear CCP1IF for next edge
		while(PIR2bits.CCP2IF == 0) //wait for next rising edge
		T1CONbits.TMR1ON = 0;	//stop TMR1
        printf("%u \r\n", 32*CCPR2);    //Accuracy is good when f_in is multiple of 32 uS
        __delay_ms(100);
	}
}
