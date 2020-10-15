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


/* Configure the data bus and Control bus as per the hardware connection
   Data bus is connected to PB0:PB7 and control bus PD0:PD4*/
#define GlcdDataBus      PORTB
#define GlcdControlBus   PORTD

#define GlcdDataBusDirnReg   TRISB
#define GlcdCtrlBusDirnReg   TRISD

#define RS     0
#define RW     1
#define EN     2
#define CS1    3
#define CS2    4

/* 5x7 Font including 1 space to display HELLO WORLD */
char H[]={0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00};
char E[]={0x7F, 0x49, 0x49, 0x49, 0x41, 0x00};
char L[]={0x7F, 0x40, 0x40, 0x40, 0x40, 0x00};
char O[]={0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00};

char W[]={0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00};
char R[]={0x7F, 0x09, 0x19, 0x29, 0x46, 0x00};
char D[]={0x7F, 0x41, 0x41, 0x22, 0x1C, 0x00};

char blank[]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void Glcd_SelectHL() // CS1=1, CS2=0  (Half-left)
{
    GlcdControlBus |= (1<<CS1);
    GlcdControlBus &= ~(1<<CS2);
}

void Glcd_SelectHR() // CS1=0, CS1=1 (Half-right)
{
    GlcdControlBus &= ~(1<<CS1);
    GlcdControlBus |= (1<<CS2);
}

/* Function to send the command to LCD */
void Glcd_CmdWrite(char cmd)
{
    GlcdDataBus = cmd;           //Send the Command nibble
    GlcdControlBus &= ~(1<<RS);  // Send LOW pulse on RS pin for selecting Command register
    GlcdControlBus &= ~(1<<RW);  // Send LOW pulse on RW pin for Write operation
    GlcdControlBus |= (1<<EN);   // Generate a High-to-low pulse on EN pin
    __delay_ms(5);
    GlcdControlBus &= ~(1<<EN);

    __delay_ms(10);
}

/* Function to send the Data to LCD */
void Glcd_DataWrite(char dat)
{
    GlcdDataBus = dat;           //Send the data on DataBus nibble
    GlcdControlBus |= (1<<RS);   // Send HIGH pulse on RS pin for selecting data register
    GlcdControlBus &= ~(1<<RW);  // Send LOW pulse on RW pin for Write operation
    GlcdControlBus |= (1<<EN);   // Generate a High-to-low pulse on EN pin
    __delay_ms(5);
    GlcdControlBus &= ~(1<<EN);

    __delay_ms(10);
}

void Glcd_DisplayChar(char *ptr_array)
{
    int i;
    for(i=0;i<6;i++) // 5x7 font, 5 chars + 1 blank space
        Glcd_DataWrite(ptr_array[i]);
}

void Glcd_ClearScreen(void)
{
    
}


int main()
{
	unsigned char i;
	
    GlcdDataBusDirnReg = 0x00;  // Configure all the LCD pins as output
    GlcdCtrlBusDirnReg = 0x00;  // Configure the Ctrl pins as output

    /* Select the half left/half right and Turn on the GLCD */
    Glcd_SelectHL();
    Glcd_CmdWrite(0x3f);
    Glcd_SelectHR();
    Glcd_CmdWrite(0x3f);
    __delay_ms(5);

    /* Select the HL/HR and Enable the GLCD */
    Glcd_SelectHL();
    Glcd_CmdWrite(0xc0);
    Glcd_SelectHR();
    Glcd_CmdWrite(0xc0);
    __delay_ms(5);
    
	/* Clear Area for Display Text */
    Glcd_SelectHL(); 			// Select HL
    Glcd_CmdWrite(0xb8);       	//Line1
    Glcd_CmdWrite(0x40);       	//Col 0 
	for(i = 1; i <= 8; i++)
		Glcd_DisplayChar(blank);    //turn off all pixels
        
       
    Glcd_SelectHL(); // Display HELLO on HL, Page1 (Line1)
    Glcd_CmdWrite(0xb8);
    Glcd_CmdWrite(0x40);
    Glcd_DisplayChar(H);
    Glcd_DisplayChar(E);
    Glcd_DisplayChar(L);
    Glcd_DisplayChar(L);
    Glcd_DisplayChar(O);

    Glcd_SelectHR(); // Display WORLD on HR, Last line (Page 7)
    Glcd_CmdWrite(0xbF);
    Glcd_CmdWrite(0x40);
    Glcd_DisplayChar(W);
    Glcd_DisplayChar(O);
    Glcd_DisplayChar(R);
    Glcd_DisplayChar(L);
    Glcd_DisplayChar(D);

    while(1) {
        
    };
}
