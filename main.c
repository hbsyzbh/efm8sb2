#include "si_efm8sb2_register_enums.h"
/*
Status Register Read D7h
Manufacturer and Device ID Read 9Fh
Configure “Power of 2” (Binary) Page Size 3Dh + 2Ah + 80h + A6h
Main Memory Page Program through Buffer 1 with Built-In Erase 82h
Main Memory Page Program through Buffer 2 with Built-In Erase 85h
Continuous Array Read (Low Power Mode) 01h
*/
//sysclk =20m /8 
//------------timer0 1 SYSCLK_DIV_12 System clock divided by 12.
//The Timer 1 reload value and prescaler should be set so that overflows 
//occur at twice the desired UART0 baud rate. The UART0 baud rate is half of the Timer 1 overflow rate.
//char rxbuff[16] = {0};
char i = 0;
char j = 0;
code char str[]="Bob.Zhu@Maxdone.com.cn\r\n";
code char helpstr[]="?: Help\r\nF: Flash\r\nM: FlashManufacturer\r\n";
char lastchar;
bool gotnewchar = 0;
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

//Configure “Power of 2” (Binary) Page Size 3Dh + 2Ah + 80h + A6h
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

#if 0 //
xdata unsigned char cc113config[48];
unsigned char cc113status;

void setcc113IO()
{
	char i;
	P0_B3 = 1;
	//dealy(10);
	P0_B3 = 0;
	
		SPI0DAT = 0x40;
		while( ! SPI0CN0_SPIF);
		
		cc113status = SPI0DAT;
		SPI0CN0_SPIF = 0;	
	
	
	for (i = 0; i < 3; i++) {
		SPI0DAT = 0x2E;
		while( ! SPI0CN0_SPIF);

		SPI0CN0_SPIF = 0;
	}
	
	P0_B3 = 1;
}

void checkcc113()
{
	char i;
	P0_B3 = 1;
	//dealy(10);
	P0_B3 = 0;
	
		SPI0DAT = 0xC0;
		while( ! SPI0CN0_SPIF);
		
		cc113status = SPI0DAT;
		SPI0CN0_SPIF = 0;	
	
	
	for (i = 0; i < 48; i++) {
		SPI0DAT = 0xC0;
		while( ! SPI0CN0_SPIF);
		
		cc113config[i] = SPI0DAT;
		SPI0CN0_SPIF = 0;
	}
	
	P0_B3 = 1;
}
#endif

void BoardInit()
{
		PCA0MD = PCA0MD_WDTE__DISABLED;			//关狗

#if 1	
	CLKSEL = CLKSEL_CLKDIV__SYSCLK_DIV_8 | CLKSEL_CLKSL__LPOSC;  //默认20M/8  SYS_CLK
#else
	HFO0CN = 0x8F;
	while(HFO0CN != 0xCF) ;
	CLKSEL = CLKSEL_CLKDIV__SYSCLK_DIV_8 | CLKSEL_CLKSL__HFOSC;
#endif
	
	CKCON0 = CKCON0_T1M__SYSCLK;	//TIMER1 使用 SYS_CLK
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
	
	IE = IE_EA__ENABLED | IE_ES0__ENABLED;
}

unsigned char uart0TxIndex = 0;
unsigned char uart0TxLen;
xdata unsigned char uart0TxBuff[256];


#define uart0RxBuffSize (512)
unsigned short uart0RxIndex = 0;
xdata unsigned char  uart0RxBuff[uart0RxBuffSize];

void uart0() interrupt UART0_IRQn
{
	if (SCON0_TI) 
	{
		if (uart0TxIndex < uart0TxLen) {
				SBUF0 = uart0TxBuff[uart0TxIndex++];
		}
		SCON0_TI = 0;
	}		

	if (SCON0_RI) 
	{
		lastchar = SBUF0;
		SCON0_RI = 0;
		gotnewchar = 1;
	}
}

void copy(unsigned char *des, unsigned char *src, unsigned short len)
{
	unsigned char i;
	for(i =0; i< len; i++)
	{
			des[i] = src[i];
	}
}

int StrSize(char *str)
{
	int i = 0;
	while(str[i++] != 0) ;

		return i;
}

void uart0sendByte(unsigned char byte)
{
	uart0TxLen = 0;
	SCON0_TI = 0;
	uart0TxIndex = 0;
	uart0TxLen = 0;
	SBUF0 = byte;
}

void uart0send(unsigned char *str, unsigned char len)
{
	uart0TxLen = 0;
	SCON0_TI = 0;
	copy(uart0TxBuff, str, len);
	uart0TxIndex = 0;
	uart0TxLen = len;
	SBUF0 = uart0TxBuff[uart0TxIndex++];
}

/*
	{
			
			if (lastchar == 'F' ) {
				SBUF0 = buff[buffindex++];
			} else if (lastchar == 'C' ) {
				SBUF0 = cc113config[configindex++];
			} else if (lastchar == 'S' ) {
				SBUF0 = cc113status;
			} else {
				SBUF0 = str[index++];
			}
			
			if (index >= 24) index = 0;
			if (configindex >= 48) {
				configindex = 0;
			}
			
		}
*/
void XmodemFlash(unsigned char *buff, unsigned short packetNum)
{
	static xdata unsigned char flashBuff[256];
	if ((packetNum % 2) == 1) {
		copy(flashBuff, buff, 128);
	} else {
		copy(&flashBuff[128], buff, 128);
		//void WritetoFlash(unsigned long address, char *buff, int len)
		WritetoFlash((packetNum / 2 - 1) * 256, flashBuff, 256);
	}
}

#define SOH 0x01 
#define STH 0x02 
#define EOT 0x04  
#define ACK 0x06  
#define NAK 0x15  
void doFlashDownload()
{
	unsigned char i;
	unsigned short packetNum;
	xdata unsigned char rxBuff[140];
	
	uart0sendByte(ACK);
	i = 0;
	packetNum = 1;
	for(;;) {
		if (gotnewchar)
		{
			rxBuff[i++] = lastchar;
			gotnewchar = 0;
			if (i == 1) {
				if (rxBuff[0] == EOT) {
					uart0sendByte(EOT);
					break;
				} else if (rxBuff[0] == SOH) {
						;
				} else {
						break;
				}
			}
			
			if (i== 133) {
				unsigned char Num = packetNum;
				if ((rxBuff[0] == SOH) && (rxBuff[1] == Num) && (rxBuff[2] == ~Num)) {
					  XmodemFlash(&rxBuff[3], packetNum);
						uart0sendByte(ACK);
						packetNum++;
				} else {
					 uart0sendByte(NAK);
				}
				i = 0;
			}
		}
	}
}

int main()
{

	char index = 0;
	unsigned char buffindex = 0;
	unsigned char configindex = 0;
	int loop = 0;
	bool dump = 0;
 
	BoardInit();
	
	uart0send(str, StrSize(str) - 1);
		
	get_deviceid();
	
	for(;;)
	{
		if (gotnewchar)
		{
			gotnewchar = 0;
			
			switch(lastchar)
			{
				case 'F':
				case 'f':
					uart0send(buff, 255);
					break;
				
				case 'C':
					doFlashDownload();
					break;
				
				case 'M':
				case 'm':
					uart0send(Manufacturer, 6);
					break;
				
				case '?':
					uart0send(helpstr, StrSize(helpstr) - 1);
					break;
				
				default:
					uart0send(str, StrSize(str) - 1);
					break;
					
			}
		}
	}
		
}