#include "si_efm8sb2_register_enums.h"
/*
Status Register Read D7h
Manufacturer and Device ID Read 9Fh
Configure ¡°Power of 2¡± (Binary) Page Size 3Dh + 2Ah + 80h + A6h
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
code char helpstr[]="?: Help\r\nF: Flash\r\nM: FlashManufacturer\r\nT: Timer4sec\r\nP: Play\r\nOther: Author\r\n";
char lastchar;
bool gotnewchar = 0;
#define BUF_LEN 256
xdata unsigned char buff[BUF_LEN] = {0};
unsigned char Manufacturer[6] = "      ";
bool sec4 = 0;
bool testtimer = 0;
bool bPlaying = 0;

#define WAV_LEN_INDEX	(40)

unsigned long wavelen = 0;
unsigned long waveindex = WAV_LEN_INDEX + 4;
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

//Continuous Array Read (Low Power Mode) 01h
void ReadtoBuff_forint(unsigned long address, char *buff, int len)
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


unsigned char getFlashByte(unsigned long address)
{
	 unsigned char bytesbuff;
		ReadtoBuff(address, &bytesbuff, 1);
	
	return bytesbuff;
}

unsigned char getFlashByte_forint(unsigned long address)
{
	 unsigned char bytesbuff;
		ReadtoBuff_forint(address, &bytesbuff, 1);
	
	return bytesbuff;
}


//Configure ¡°Power of 2¡± (Binary) Page Size 3Dh + 2Ah + 80h + A6h
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

void LM4991_forint(bool on)
{
	if (on){
		P1_B6 = 1;
		P1_B5 = 0;
		PCA0CN0 = PCA0CN0_CR__RUN;
	}else{
		P1_B6 = 0;
		P1_B5 = 1;
		PCA0CN0 = PCA0CN0_CR__STOP;
	}
}

void LM4991(bool on)
{
	if (on){
		P1_B6 = 1;
		P1_B5 = 0;
		PCA0CN0 = PCA0CN0_CR__RUN;
	}else{
		P1_B6 = 0;
		P1_B5 = 1;
		PCA0CN0 = PCA0CN0_CR__STOP;
	}
}

void SetDuty(unsigned char duty)
{
	PCA0CPH0 = duty;
}

void BoardInit()
{
		PCA0MD = PCA0MD_WDTE__DISABLED | PCA0MD_CPS__SYSCLK;

#if 1	
	CLKSEL = CLKSEL_CLKDIV__SYSCLK_DIV_8 | CLKSEL_CLKSL__LPOSC;  //Ä¬ÈÏ20M/8  SYS_CLK
#else
	HFO0CN = 0x8F;
	while(HFO0CN != 0xCF) ;
	CLKSEL = CLKSEL_CLKDIV__SYSCLK_DIV_8 | CLKSEL_CLKSL__HFOSC;
#endif
	
	//CKCON0 = CKCON0_T1M__SYSCLK;	//TIMER1 Ê¹ÓÃ SYS_CLK
	CKCON0 = CKCON0_T1M__SYSCLK | CKCON0_T3ML__SYSCLK | CKCON0_T3MH__SYSCLK;
	TMOD = TMOD_T1M__MODE2;
	TL1 = TH1 = 126;
	//TL1 = TH1 = 97;
	SCON0 = 0x50;
	
	TCON = TCON_TR1__RUN;
	
	SPI0CFG = SPI0CFG_MSTEN__MASTER_ENABLED;
	SPI0CN0 = SPI0CN0_NSSMD__3_WIRE | SPI0CN0_SPIEN__ENABLED;
	
	XBR0 = XBR0_URT0E__ENABLED | XBR0_SPI0E__ENABLED | XBR0_CP0E__ENABLED;
	XBR2 = XBR2_XBARE__ENABLED;
	P0MDOUT = P0MDOUT_B0__PUSH_PULL | P0MDOUT_B1__OPEN_DRAIN | P0MDOUT_B2__PUSH_PULL | P0MDOUT_B3__PUSH_PULL |
						P0MDOUT_B4__PUSH_PULL | P0MDOUT_B5__OPEN_DRAIN | P0MDOUT_B6__PUSH_PULL | P0MDOUT_B7__PUSH_PULL ;
						
	P0SKIP =  P0SKIP_B0__NOT_SKIPPED | P0SKIP_B1__NOT_SKIPPED
					| P0SKIP_B2__NOT_SKIPPED | P0SKIP_B3__SKIPPED
					| P0SKIP_B4__NOT_SKIPPED | P0SKIP_B5__NOT_SKIPPED
					| P0SKIP_B6__SKIPPED     | P0SKIP_B7__SKIPPED;
	
	P1MDOUT = P1MDOUT_B0__PUSH_PULL | P1MDOUT_B1__PUSH_PULL | 
						P1MDOUT_B4__PUSH_PULL | P1MDOUT_B5__PUSH_PULL | P1MDOUT_B5__PUSH_PULL;

	P1SKIP =  P1SKIP_B0__SKIPPED 		 | P1SKIP_B1__SKIPPED 
					| P1SKIP_B2__SKIPPED		 | P1SKIP_B3__SKIPPED;
	
	IE = IE_EA__ENABLED | IE_ES0__ENABLED;
	EIE1 = EIE1_ET3__ENABLED;
	TMR3CN0 = TMR3CN0_TR3__RUN | TMR3CN0_T3SPLIT__16_BIT_RELOAD;
	
	PCA0PWM = PCA0PWM_CLSEL__8_BITS | PCA0PWM_ARSEL__AUTORELOAD;
	PCA0CPM0 = PCA0CPM0_PWM__ENABLED;
	SetDuty(0);
	LM4991(0);
}

//#define FREQ 11025
// 20M/8 - 305.2k/8     2.5M - 38.15
//void selectTimer3Freq(unsigned int freq)
void selectTimer3Freq(void)
{
/*	unsigned short time = 2500000 / freq;
	time = 65536 - time;

	TMR3RLH = TMR3H = time / 256;
	TMR3RLL = TMR3L = time % 256;
	TMR3RL = time;
*/	
	TMR3 = TMR3RL = 65308;
}

void timer3() interrupt TIMER3_IRQn
{
	static unsigned short count = 0;
	TMR3CN0 = TMR3CN0_TR3__RUN | TMR3CN0_T3SPLIT__16_BIT_RELOAD;
	count++;
	if ( count == 44100 ) {
		count = 0;
		sec4 = 1;
	}
	
	if (bPlaying) {
		if (waveindex < wavelen) {
			SetDuty(getFlashByte_forint(waveindex++));
		} else {
			LM4991_forint(0);
			bPlaying = 0;
		}
	}
}

void Play()
{
	LM4991(1);
	waveindex = WAV_LEN_INDEX + 4;
	SetDuty(getFlashByte(waveindex++));
	bPlaying = 1;
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

int StrSize(char *pstr)
{
	int i = 0;
	while(pstr[i++] != 0) ;

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
			unsigned char Num = packetNum;
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
			
			if (i == 2) {
				if (rxBuff[1] != Num) {
						break;
				}
			}
			
			if (i == 3) {
				if (rxBuff[2] != ~Num) {
					break;
				}
			}
			
			if (i== 133) {
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

void int2str(char *str, unsigned long v)
{
	bool bFirstNotZero = 0;
	unsigned char i = 0;
	
	str[i] = (v / 1000000000)+ '0';
	v = v %1000000000;
	if (str[i] != '0') {
		bFirstNotZero = 1;
		i++;
	}	
	
	str[i] = (v / 100000000) + '0';
	v = v %100000000;
	if (bFirstNotZero || (str[i] != '0')) {
		bFirstNotZero = 1;
		i++;
	}
	
		str[i] = (v / 10000000) + '0';
	v = v %10000000;
	if (bFirstNotZero || (str[i] != '0')) {
		bFirstNotZero = 1;
		i++;
	}
	
		str[i] = (v / 1000000) + '0';
	v = v %1000000;
	if (bFirstNotZero || (str[i] != '0')) {
		bFirstNotZero = 1;
		i++;
	}
	
		str[i] = (v / 100000) + '0';
	v = v %100000;
	if (bFirstNotZero || (str[i] != '0')) {
		bFirstNotZero = 1;
		i++;
	}
	
		str[i] = (v / 10000) + '0';
	v = v %10000;
	if (bFirstNotZero || (str[i] != '0')) {
		bFirstNotZero = 1;
		i++;
	}
	
		str[i] = (v / 1000) + '0';
	v = v %1000;
	if (bFirstNotZero || (str[i] != '0')) {
		bFirstNotZero = 1;
		i++;
	}
	
			str[i] = (v / 100) + '0';
	v = v %100;
	if (bFirstNotZero || (str[i] != '0')) {
		bFirstNotZero = 1;
		i++;
	}
	
				str[i] = (v / 10) + '0';
	v = v %10;
	if (bFirstNotZero || (str[i] != '0')) {
		bFirstNotZero = 1;
		i++;
	}
	
	str[i] = (v / 1) + '0';

	if (bFirstNotZero || (str[i] != '0')) {
		bFirstNotZero = 1;
		i++;
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
	//selectTimer3Freq(FREQ);
	selectTimer3Freq();
	
	//wavelen = *((unsigned long *)(&buff[WAV_LEN_INDEX]));
	
	wavelen = buff[WAV_LEN_INDEX] ;
	wavelen += buff[WAV_LEN_INDEX + 1] << 8;
	wavelen += buff[WAV_LEN_INDEX + 2] << 16;
	wavelen += buff[WAV_LEN_INDEX + 3] << 24;
	Play();
	for(;;)
	{
		if (sec4 && testtimer) {
			sec4 = 0;
			uart0send(str, StrSize(str) - 1);
		}
		
		if (gotnewchar)
		{
			gotnewchar = 0;
			testtimer = 0;
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
				
				case 'P':
				case 'p':
				  {
						 xdata char intstr[32] = {0};
							
						int2str(intstr, wavelen);
						intstr[StrSize(intstr) - 1] = '\r';
						intstr[StrSize(intstr) - 1] = '\n';
					  uart0send(intstr, StrSize(intstr) - 1);
					}
					Play();
					break;
				
				case 'T':
				case 't':
					testtimer = 1;
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