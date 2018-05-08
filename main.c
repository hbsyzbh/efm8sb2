#include "si_efm8sb2_register_enums.h"

//sysclk =20m /8 
//timer0 1 SYSCLK_DIV_12 System clock divided by 12.
//The Timer 1 reload value and prescaler should be set so that overflows 
//occur at twice the desired UART0 baud rate. The UART0 baud rate is half of the Timer 1 overflow rate.
char buff[16] = {0};
char i = 0;
char j = 0;
char str[]="Maxdone";
int main()
{
	CLKSEL = CLKSEL_CLKDIV__SYSCLK_DIV_8 | CLKSEL_CLKSL__LPOSC;  //默认20M/8  SYS_CLK
	PCA0MD = PCA0MD_WDTE__DISABLED;			//关狗
	
	CKCON0 = CKCON0_T1M__SYSCLK;	//TIMER1 使用 SYS_CLK
	TMOD = TMOD_T1M__MODE2;
	TL1 = TH1 = 126;
	SCON0 = 0x50;
	
	TCON = TCON_TR1__RUN;
	
	XBR0 = 0x01;
	XBR2 = 0x40;
	P0MDOUT = 0x10;
	
	SBUF0 = 0xa5;
	for(;;)
	{
		if (SCON0_TI) 
		{
			SCON0_TI = 0;
			SBUF0 = str[j++ % sizeof(str)];
		}
		
		if (SCON0_RI) 
		{
			SCON0_RI = 0;
			buff[((i++) % 16)] = SBUF0;
		}
		
	}
}