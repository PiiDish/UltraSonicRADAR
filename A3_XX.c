/*_____________________________________________________________________________________________________________________

        Project:        
        Module:         
        Author:         
        Date:           

        Description:
        This program ....

        Notes:          

        Revisions:


_______________________________________________________________________________________________________________________*/
#include <stdlib.h>
#include <stdio.h>
#include <compiler_defs.h>
#include <C8051F120_defs.h>
//#include <c8051f120.h>     // SFR declarations
#include "A3_XX.h"

int angle;
unsigned char vector;
unsigned char distime;
signed char NthSth;
bit alarm;
/*--------------------------------------------------------------------------------------------------------------------
        Function:         Main

        Description:      Main routine

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void main(void)
{
	P2=0x00;
	angle=0;		//0 degrees
	distime=0;		//Time equals 0
	alarm=0;
	General_Init();
	DAC_Init();
	Timer_Init();
	LCD_Init();	
	Interrupts_Init();
	SFRPAGE=TIMER01_PAGE;
	//TR0=1;
	//SplashScreen();

	
	while(1)
	{
		Sweep();
		//mSec_Delay(1000);
		angle+=NthSth;
	}
/*
		// Do stuff
		if(~PB1){
		SFRPAGE   = TIMER01_PAGE;
			TR1=0;
			SFRPAGE   = TMR4_PAGE;
			TR4=1;
			Xms_Delay(1000);
			TR4=0;
			}
		if(~PB2){
			
			}
		if(~PB3)
			{
				
			}
		if(~PB4){
			Xms_Delay(20);
			if(PB4){
			}
		}
		if(~PB5){
			Xms_Delay(30);
		}
		if(~PB6){
			uSec_Delay(50);
			SplashScreen();
			}
		if(~PB7){
			uSec_Delay(50);
			if(PB7)
				{
				}
			}
		if(~PB8)
		{
			Ping();
		}
		LD1=TR4;
	}
*/
}
/*--------------------------------------------------------------------------------------------------------------------
        Function:         General_Init

        Description:      Initialise ports, watchdog....

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void General_Init()
{
	WDTCN = 0xde;
	WDTCN = 0xad;
  	SFRPAGE = CONFIG_PAGE;
	P0MDOUT = 0x10;		// NOTE: Pushpull required for Servo control OTHERWISE TOO WEAK TO DRIVE PROPERLY SINCE ONLY 3.3V!!!!
	P1MDOUT = 0x00;		// Ensure not pushpull outputs....this could damage microcontroller...
	P2MDOUT = 0xff;		// Need to make pushpull outputs to drive LEDs properly
	
	OSCICN    = 0x83;	// Need a faster clock....24.5MHz selected
	
	XBR0 = 0x04;
    XBR1 = 0x60;		//Crossbar for interupt to trigger to port 0 pin3TX and pin4RX
	XBR2 = 0x40;
	Servo_Ctrl = 0;	// Initialise servo control pin to 0
	USonicRX=0;
}
/*--------------------------------------------------------------------------------------------------------------------
        Function:         DAC_Init

        Description:      Initialise DAC0. 

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void DAC_Init()
{
  	SFRPAGE   = DAC0_PAGE;
	DAC0CN    = 0x90;
}
/*--------------------------------------------------------------------------------------------------------------------
        Function:        ADC_Init

        Description:      Initialise ADC0. 

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Voltage_Reference_Init()
{
    SFRPAGE   = ADC0_PAGE;
    REF0CN    = 0x12;
}
/*--------------------------------------------------------------------------------------------------------------------
        Function:         Timer_Init

        Description:      

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Timer_Init()
{
	// Initialise timers
	SFRPAGE   = TIMER01_PAGE;
    TCON      = 0x40;
    TMOD      = 0x11;	//Timer0 and Timer1
    TL0       = 0xde;		//Low byte 20mSec
    TL1       = 0x00;		//overflow for uart
    TH0       = 0xb1;		//High byte 20mSec
    TH1       = 0x00;		//Overflow for uart

    SFRPAGE   = TMR2_PAGE;	//Timer2
    //TMR2CN    = 0x04;
    TMR2CF    = 0x0A;
    RCAP2L    = 0xce;		//4033Hz
    RCAP2H    = 0xfe;
    TMR2L     = 0xce;
    TMR2H     = 0xfe;


	SFRPAGE   = TMR3_PAGE;	//Timer3 capture mode
	//TMR3CN    = 0x09;
    TMR3CF    = 0x08;
	TMR3L     = 0xe6;		//For 1uSec count for distime
    TMR3H     = 0xff;
	RCAP3L    = 0xe6;
    RCAP3H    = 0xff;

	SFRPAGE   = TMR4_PAGE;	//Timer4
    //TMR4CN    = 0x04;
    TMR4CF    = 0x0A;
    RCAP4L    = 0xf5;		//260Hz
    RCAP4H    = 0x47;
    TMR4L     = 0x00;
    TMR4H     = 0x00;
}

/*--------------------------------------------------------------------------------------------------------------------
        Function:         LCD_Init

        Description:      Initialise LCD. Note that the LCD has a 4 bit hardware connection 

									P3.7	BL	=> Backlight		
									P3.6	E	=> Enable		
									P3.5	R/W	=> Read/Write line		
									P3.4	RS	=> Register Select		
									P3.3	D7	=> Data line		
									P3.2	D6	=> Data line		
									P3.1	D5	=> Data line		
									P3.0	D4	=> Data line															

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void LCD_Init()
{
	// Initialise LCD
	LCD = 0x00;

	mSec_Delay(20);			// 	20ms delay
	LCD = 0x03;				//	Function Set	
	Toggle_E();				// 	Toggle the E line
	mSec_Delay(2);			// 	2ms delay	

	LCD = 0x03;				//	Function Set	
	Toggle_E();				// 	Toggle the E line
	mSec_Delay(2);			// 	1ms delay	

	LCD = 0x03;				//	Function Set	
	Toggle_E();				// 	Toggle the E line
	mSec_Delay(2);			// 	1ms delay	

	LCD = 0x02;				//	Function Set	
	Toggle_E();				// 	Toggle the E line

	LCD = 0x02;				//	Function Set	
	Toggle_E();				
	LCD = 0x08;				//	2 lines, 5x8 display	
	Toggle_E();				

	LCD = 0x00;				//	Display off	
	Toggle_E();				
	LCD = 0x08;					
	Toggle_E();					

	LCD = 0x00;				//	Display clear display	
	Toggle_E();				
	LCD = 0x01;					
	Toggle_E();	

	LCD = 0x00;				//	Cursor increment
	Toggle_E();				
	LCD = 0x06;					
	Toggle_E();	
	
	LCD = 0x00;				//	Display on
	Toggle_E();				
	LCD = 0x0C;					
	Toggle_E();

	LCD_Write_String("<<Initialised>>");
	mSec_Delay(1000);
	LCD_Send_Cmd(0x01);
}

/*--------------------------------------------------------------------------------------------------------------------
        Function:         Interrupts_Init

        Description:      Initialise interrupts

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Interrupts_Init()
{
    // Set up interrupts if required
	IE=0xB2;//A;	//Timer interrupts enabled for TMR0 TMR1 TMR2
	EIE2=0x01;//5;	//Timer interrupts enabled for TMR3 TMR4
}
//--------------------------------------------------------------------------------------------------------------------	
/*--------------------------------------------------------------------------------------------------------------------
        Function:         Timer0_ISR

        Description:      Timer 0 interrupt service routine

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Timer0_ISR (void) interrupt 1
{
  	unsigned char SFRPAGE_SAVE = SFRPAGE; // Save Current SFR page
	static unsigned char count;

	if(count == 10)
	{
		count=0;
		distime=650;
		LD0=~LD0;
	}
	Servo_Ctrl=1;
	uSec_Delay(600+(angle*10));		//600uSecs equals 0 degrees
	Servo_Ctrl=0;

	if(angle==180)
		NthSth=-1;
	else if(angle==0)
		NthSth=1;

	TL0=0xde;
	TH0=0xb1;
	//angle+=NthSth;
	count++;


  	SFRPAGE = SFRPAGE_SAVE; 			// Restore SFR page
}
/*--------------------------------------------------------------------------------------------------------------------

        Function:         Timer1_ISR

        Description:      Timer 1 interrupt service routine

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Timer1_ISR (void) interrupt 3
{
  unsigned char SFRPAGE_SAVE = SFRPAGE; // Save Current SFR page

	//UART to go here

  	SFRPAGE = SFRPAGE_SAVE; 			// Restore SFR page
}
/*--------------------------------------------------------------------------------------------------------------------
        Function:         Timer2_ISR

        Description:      Timer 2 interrupt service routine

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Timer2_ISR (void) interrupt 5
{
  unsigned char SFRPAGE_SAVE = SFRPAGE; // Save Current SFR page
	
	// Do stuff here...
	int i;
	for(i=0;i<4;i++)
	{
		USonicTX=~USonicTX;
		LD1=USonicTX;
		uSec_Delay(10);
		
	}
	//Start timer here for distance time
	//SFRPAGE=TMR3_PAGE;
	//TR3=1;
	//while(USonicRX!=1);
		
	//while(TF2!=1);
	TR2=0;
	TF2=0;       					// Reset interrupt flag
  	SFRPAGE = SFRPAGE_SAVE; 			// Restore SFR page
}
/*--------------------------------------------------------------------------------------------------------------------
        Function:         Timer3_ISR

        Description:      Timer 3 interrupt service routine

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Timer3_ISR (void) interrupt 14
{
	unsigned short int dTime;
  	unsigned char SFRPAGE_SAVE = SFRPAGE; // Save Current SFR page
	SFRPAGE=TMR3_PAGE;	
	
	//dTime=
	//dTime=RCAP3H<<8;
	//dTime&=0xff00;
	//dTime|=RCAP3L;
	//dTime=RCAP3;
	//distime=0;
	//TMR3L = 0xbc;
    //TMR3H = 0xe8;

	//angle+=NthSth;

	distime++;	//Adding 1uSec to distime for time taken to target
	//TR3=0;
	//TF3=0;		// Reset interrupt flag
  	
	SFRPAGE = SFRPAGE_SAVE; 			// Restore SFR page
}
//--------------------------------------------------------------------------------------------------------------------
/*--------------------------------------------------------------------------------------------------------------------
        Function:         Timer4_ISR

        Description:      Timer 4 interrupt service routine

        Revisions:

--------------------------------------------------------------------------------------------------------------------*/
void Timer4_ISR (void) interrupt 16
{
  unsigned char SFRPAGE_SAVE = SFRPAGE; // Save Current SFR page
  	SFRPAGE=DAC0_PAGE;
	
	// Do stuff here...
	DAC0=~DAC0;
	mSec_Delay(1000);

	SFRPAGE=TMR4_PAGE; 
	
	TR4=0;
	mSec_Delay(1000);
	TR4=1;
	TF4=0;       					// Reset interrupt flag
  	
	SFRPAGE = SFRPAGE_SAVE; 			// Restore SFR page
}
//--------------------------------------------------------------------------------------------------------------------
//		1 mSec delay using timer0
//--------------------------------------------------------------------------------------------------------------------
void Xms_Delay(int j)
{
	int i=0;
	char SFRPAGE_SAVE;
	
	SFRPAGE_SAVE = SFRPAGE; // Save Current SFR page
	SFRPAGE = TIMER01_PAGE;
	for(i=0;i<j;++i)
	{
		TR0=1;
		while(TF0!=1);
		TR0=0;
		TF0=0;
		TL0=0x2f;//17;
		TH0=0xf9;//fc;
	}
	SFRPAGE = SFRPAGE_SAVE;
}
//--------------------------------------------------------------------------------------------------------------------
//		Approx 1uSec Delay 
//--------------------------------------------------------------------------------------------------------------------
void uSec_Delay(int u)
{
	int i;
	unsigned char d=0x18;	//64;	//100 decimal 			0x78;	//120 decimal
	for(i=0;i<u;++i)
		while(d!=0)
			d--;
}
//--------------------------------------------------------------------------------------------------------------------
//		Approx 1 microSec Delay 
//--------------------------------------------------------------------------------------------------------------------
void mSec_Delay(unsigned int uS)
{
	int i;
	for(i=0;i<uS;++i)
		uSec_Delay(1000);
}
//--------------------------------------------------------------------------------------------------------------------
//		Toggle_E
//--------------------------------------------------------------------------------------------------------------------
void Toggle_E()
{
	int i=10;
	E=1;
	while(i--);
	E=0;
	mSec_Delay(2);	//Smallest delay possible for LCD to work. This held me up for ages!!!!!
}
//---------------------------------------------------------------------------------------------------------------------
//		Write Command to LCD
//		
//---------------------------------------------------------------------------------------------------------------------
void LCD_Send_Cmd(unsigned char cmd)
{
	unsigned char tempCmd;

	RS=0;
	RW=0;

	tempCmd=cmd>>4;
	
	tempCmd&=0x0f;
	LCD&=0xf0;
	LCD|=tempCmd;
	Toggle_E();
	
	cmd&=0x0f;
	LCD&=0xf0;
	LCD|=cmd;
	Toggle_E();
}
//---------------------------------------------------------------------------------------------------------------------
//		Write Char to LCD
//		
//---------------------------------------------------------------------------------------------------------------------
void LCD_Write_Char(unsigned char ch)
{
	unsigned char tempCh;

	RS=1;			//Pull register select high to write data to LCD
	RW=0;			//Keep Read/Write low to write

	tempCh=ch>>4;		//Swap nibbles

	tempCh&=0x0f;		//And out higher nibble
	LCD &=0xf0;		//Clear lower nibble LCD for Data
	LCD|=tempCh;		//Or high nibbe to LCD
	Toggle_E();

	ch&=0x0f;		//And out higher nibble
	LCD &=0xf0;		//Clr Data line Nibble of LCD;
	LCD|=ch;
	Toggle_E();

	RS=0;			//Pull register select low
	
}
//---------------------------------------------------------------------------------------------------------------------
//		Write String to LCD
//---------------------------------------------------------------------------------------------------------------------
void LCD_Write_String(char *ch)
{
	while(*ch !=0)
		LCD_Write_Char(*ch++);	
}
//---------------------------------------------------------------------------------------------------------------------
//		int to string
//---------------------------------------------------------------------------------------------------------------------
void toScreen(int strAngle, float strTime)
{
	char buf[10];
	//char radarDisplay[13];

	sprintf(buf,"%d", strAngle);
	LCD_Send_Cmd(0x80);
	LCD_Write_String(buf);
	LCD_Write_Char(0xdf);		//degree symbol
	LCD_Send_Cmd(0xc0);
	sprintf(buf,"%4.2f", strTime);
	LCD_Write_String(buf);	
	LCD_Write_String("mm");
}
//---------------------------------------------------------------------------------------------------------------------
//		Send ping
//---------------------------------------------------------------------------------------------------------------------
unsigned int Ping()
{	
	distime=0;
	SFRPAGE = TMR2_PAGE;

	TR2=1;
	//uSec_Delay(100);//Time Mask goes here
	SFRPAGE = TMR3_PAGE;
	TR3=1;
	while(distime!=0xee)
		//while(USonicRX!=1);
			LD6=1;
		LD7=USonicRX;
	//	{	TR3=0;
	//		return distime;
	//	}
	TR3=0;
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
//		Start Splash Screen
//---------------------------------------------------------------------------------------------------------------------
void SplashScreen(void)
{
	int i;
	LCD_Write_String("PiiDish");
	LCD_Send_Cmd(0xc0);
	LCD_Write_String("Assignment 3");
	mSec_Delay(2000);
	LCD_Send_Cmd(0x01);
	LCD_Write_String("RADAR");
	LCD_Send_Cmd(0xc0);
	for(i=0;i<15;i++)
	{
		LCD_Write_Char('-');
		mSec_Delay(100);
	}
	LCD_Send_Cmd(0x01);	
}
//---------------------------------------------------------------------------------------------------------------------
//		Sweep
//---------------------------------------------------------------------------------------------------------------------
void Sweep()
{
	float time;
	//int angleIndex;
	//int memAngle[13];
	//float memTime[13];

	//angleIndex=angle/15;

	time=Ping();
	//angle+=NthSth;
	//time/=2.0;
	time*=MICRO_SECS_TO_MILLIMETERS;

	//if(angle%15==0)
	//{
	//	memAngle[angleIndex]=angle;
	//	memTime[angleIndex]=time;
	//}

	toScreen(angle, time);
	//angle+=NthSth;
}
//---------------------------------------------------------------------------------------------------------------------
//		Sweep
//---------------------------------------------------------------------------------------------------------------------
void ScanTimer(unsigned int us)
{
	int n;
	unsigned char SFRPAGE_SAVE = SFRPAGE; // Save Current SFR page
	
  	SFRPAGE = TIMER01_PAGE; 		
	for(n=0;n<us;n++)
	{
		//TR1=1;
		//Servo_Ctrl=1;
    		while(TF1!=1);
    	
		TF1=0;
    		TR1=0;
		TH1=0xFF;
    		TL1=0xD2;
	}
	  	SFRPAGE = SFRPAGE_SAVE; 			// Restore SFR page

}










