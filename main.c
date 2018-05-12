#include "si_efm8sb2_register_enums.h"
/*
Status Register Read D7h
Manufacturer and Device ID Read 9Fh
Configure ��Power of 2�� (Binary) Page Size 3Dh + 2Ah + 80h + A6h
Main Memory Page Program through Buffer 1 with Built-In Erase 82h
Main Memory Page Program through Buffer 2 with Built-In Erase 85h
Continuous Array Read (Low Power Mode) 01h
*/
//sysclk =20m /8 
//------------timer0 1 SYSCLK_DIV_12 System clock divided by 12.
//The Timer 1 reload value and prescaler should be set so that overflows 
//occur at twice the desired UART0 baud rate. The UART0 baud rate is half of the Timer 1 overflow rate.
char rxbuff[16] = {0};
char i = 0;
char j = 0;
char str[]="Bob.Zhu@Maxdone.com.cn\r\n";
char lastchar;
#define BUF_LEN 256
xdata char buff[BUF_LEN] = {0};
unsigned char Manufacturer[6] = "      ";

#if 0
void dealy(char t)
{
	while(t--);
}
#endif

//Main Memory Page Program through Buffer 2 with Built-In Erase 85h
void WritetoFlash(unsigned long address, char *buff, int len)
{
	int i;
	unsigned char dummy, addr[3];
	
	//if (( 0 == buff) || (len <= 0)) return;
	
	addr[0] = (address >> 16) % 256;
	addr[1] = (address >> 8) % 256;
	//addr[2] = (address)  % 256;
	addr[2] = 0;
	
	P1_B0 = 1;
	P1_B1 = 1;

	P1_B1 = 0;
	
	SPI0DAT = 0x85;
	while( ! SPI0CN0_SPIF);
	dummy = SPI0DAT;
	SPI0CN0_SPIF = 0;

	for( i = 0; i < 3; i++) {
		SPI0DAT = addr[i];
		while( ! SPI0CN0_SPIF);
		dummy = SPI0DAT;
		SPI0CN0_SPIF = 0;
	}
	
	
	
	for( i = 0; i < len; i++) {
		SPI0DAT = buff[i];
		while( ! SPI0CN0_SPIF);
		//buff[i] = SPI0DAT;
		SPI0CN0_SPIF = 0;
	}
	
	P1_B1 = 1;
}


//Continuous Array Read (Low Power Mode) 01h
void ReadtoBuff(unsigned long address, char *buff, int len)
{
	int i;
	unsigned char dummy, addr[3];
	
	//if (( 0 == buff) || (len <= 0)) return;
	
	addr[0] = (address >> 16) % 256;
	addr[1] = (address >> 8) % 256;
	addr[2] = (address)  % 256;
	
	P1_B0 = 1;
	P1_B1 = 1;

	P1_B1 = 0;
	
	SPI0DAT = 0x01;
	while( ! SPI0CN0_SPIF);
	dummy = SPI0DAT;
	SPI0CN0_SPIF = 0;

	for( i = 0; i < 3; i++) {
		SPI0DAT = addr[i];
		while( ! SPI0CN0_SPIF);
		dummy = SPI0DAT;
		SPI0CN0_SPIF = 0;
	}
	
	
	
	for( i = 0; i < len; i++) {
		SPI0DAT = 0x01;
		while( ! SPI0CN0_SPIF);
		buff[i] = SPI0DAT;
		SPI0CN0_SPIF = 0;
	}
	
	P1_B1 = 1;
}

//Configure ��Power of 2�� (Binary) Page Size 3Dh + 2Ah + 80h + A6h
void ConfigureBinaryPageSize()
{
	const unsigned char cmd[4] = {0x3D, 0x2A, 0x80, 0xA6};
	int i;
	
	P1_B0 = 1;
	P1_B1 = 1;

	P1_B1 = 0;
	
	for(i = 0; i <4; i++)
	{
		SPI0DAT = cmd[i];
		while( ! SPI0CN0_SPIF);

		//Result[i] = SPI0DAT;
		SPI0CN0_SPIF = 0;
	}	
	
	P1_B1 = 1;
}

unsigned int getStatusRegister()
{
	unsigned int result;
	P1_B0 = 1;
	P1_B1 = 1;
	
	P1_B1 = 0;
	
		SPI0DAT = 0xD7;
		while( ! SPI0CN0_SPIF);
		result = SPI0DAT * 256;
		SPI0CN0_SPIF = 0;

		SPI0DAT = 0xD7;
		while( ! SPI0CN0_SPIF);
		result = SPI0DAT * 256;
		SPI0CN0_SPIF = 0;


		SPI0DAT = 0xD7;
		while( ! SPI0CN0_SPIF);
		result += SPI0DAT;
		SPI0CN0_SPIF = 0;
	
	P1_B1 = 1;	
	
	return result;
}

void get_deviceid()
{
	static bool bgot = 0;
	int StatusRegister;
	int i;
	
	if(bgot) return;
	
	P1_B0 = 1;
	P1_B1 = 1;
	//dealy(10);
	P1_B1 = 0;
	//dealy(100);
	
	for(i =0; i<6; i++)
	{
		SPI0DAT = 0x9F;
		while( ! SPI0CN0_SPIF);
		
		Manufacturer[i] = SPI0DAT;
		SPI0CN0_SPIF = 0;
	}
	P1_B1 = 1;
	
	StatusRegister = getStatusRegister();
	
	if ((StatusRegister & 0x0100) != 0x0100 )
	{
		ConfigureBinaryPageSize();
	}
	
	ReadtoBuff(0, buff, BUF_LEN);

#if 0	
	for(i = 0; i < BUF_LEN; i++)
		buff[i] = i;
	
	WritetoFlash(0, buff, BUF_LEN);
	
	ReadtoBuff(0, buff, BUF_LEN);
#endif	
	bgot = 1;
}

int main()
{
	char index = 0;
	unsigned char buffindex = 0;
	int loop = 0;
 
	PCA0MD = PCA0MD_WDTE__DISABLED;			//�ع�

#if 1	
	CLKSEL = CLKSEL_CLKDIV__SYSCLK_DIV_8 | CLKSEL_CLKSL__LPOSC;  //Ĭ��20M/8  SYS_CLK
#else
	HFO0CN = 0x8F;
	while(HFO0CN != 0xCF) ;
	CLKSEL = CLKSEL_CLKDIV__SYSCLK_DIV_8 | CLKSEL_CLKSL__HFOSC;
#endif
	
	CKCON0 = CKCON0_T1M__SYSCLK;	//TIMER1 ʹ�� SYS_CLK
	TMOD = TMOD_T1M__MODE2;
	TL1 = TH1 = 126;
	//TL1 = TH1 = 97;
	SCON0 = 0x50;
	
	TCON = TCON_TR1__RUN;
	
	SPI0CFG = SPI0CFG_MSTEN__MASTER_ENABLED;
	SPI0CN0 = SPI0CN0_NSSMD__3_WIRE | SPI0CN0_SPIEN__ENABLED;
	
	XBR0 = XBR0_URT0E__ENABLED | XBR0_SPI0E__ENABLED;
	XBR2 = XBR2_XBARE__ENABLED;
	P0MDOUT = P0MDOUT_B0__PUSH_PULL | P0MDOUT_B1__OPEN_DRAIN | P0MDOUT_B2__PUSH_PULL | P0MDOUT_B3__PUSH_PULL |
						P0MDOUT_B4__PUSH_PULL | P0MDOUT_B5__OPEN_DRAIN | P0MDOUT_B6__PUSH_PULL | P0MDOUT_B7__PUSH_PULL ;
	
	P1MDOUT = P1MDOUT_B0__PUSH_PULL | P1MDOUT_B1__PUSH_PULL;
	
	SBUF0 = 0xa5;
	for( ; ;loop++)
	{
		if (loop % 500)
		{
				continue;
		}
		
		get_deviceid();
		
		if (SCON0_TI) 
		{
			SCON0_TI = 0;
			if (lastchar == 'F' )
				SBUF0 = buff[buffindex++];
			else
				SBUF0 = str[index++];
			
			if (index >= 24) index = 0;
			
		}
		
		if (SCON0_RI) 
		{
			SCON0_RI = 0;
			rxbuff[((i++) % 16)] = lastchar = SBUF0;
		}
	}
}