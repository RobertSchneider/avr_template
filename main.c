#define 	F_CPU 3686400
#define	__AVR_ATmega8__
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//////////////////////////////////////////////////////////////////////////////
//	LCD-Funktionen für myAVR-Board + myAVR-LCD
//	4-BitModus an PortD Bit 4-7
//	PortD Bit 2 = RS, high=Daten, low=Kommando
//	PortD Bit 3 = E, high-Impuls für gültige Daten
//---------------------------------------------------------------------------
//	lcdSend(..) - sendet ein Byte an LCD im 4-Bit-Modus
//	RS muss vorher richtig gesetzt sein
//	PE:	data=zu sendendes Byte
//---------------------------------------------------------------------------
void lcdSend(char data)
{
	// aktuelles RS ermitteln
	char rs = PORTD;
	rs &= 4;
	// High-Teil senden
	char tmp = data;
	tmp &= 0xf0;
	tmp |= rs;
	PORTD = tmp;
	// Schreibsignal
	sbi(PORTD, 3);
	cbi(PORTD, 3);
	// Low-Teil senden
	tmp = data;
	tmp &= 0x0f;
	tmp *= 16;
	tmp |= rs;
	PORTD = tmp;
	// Schreibsignal
	sbi(PORTD, 3);
	cbi(PORTD, 3);
	// verarbeiten lassen
	waitMs(1);
}
//---------------------------------------------------------------------------
//	lcdCmd(..) - sendet ein Kommando an LCD
//	PE:	cmd=Kommando-Byte
//---------------------------------------------------------------------------
void lcdCmd(char cmd)
{
	cbi(PORTD, 2);		// RS löschen = Kommando
	lcdSend(cmd);		// senden
}
//---------------------------------------------------------------------------
//	lcdWrite(..) - sendet ein Zeichen (Daten) an LCD
//	PE:	text=Zeichen
//---------------------------------------------------------------------------
void lcdWrite1(char text)
{
	sbi(PORTD, 2);		// RS setzen = Daten
	lcdSend(text);		// senden
}
//---------------------------------------------------------------------------
//	lcdWrite(..) - sendet eine Zeichenkette an LCD
//	Die Zeichenkette muss mit 0x00 abgeschlossen sein
//	PE:	pText=Zeiger auf Zeichenkette
//---------------------------------------------------------------------------
void lcdWrite(const char* pText)
{
	while (pText[0] != 0)
	{
		lcdWrite1(pText[0]);
		pText++;
	}
}
//---------------------------------------------------------------------------
//	lcdWrite(..) - sendet eine Zeichenkette an LCD
//	PE:	pText=Zeiger auf Zeichenkette
//		count=Anzahl der zu sendenden Zeichen
//---------------------------------------------------------------------------
void lcdWrite2(const char* pText, int count)
{
	while (count != 0)
	{
		lcdWrite1(pText[0]);
		pText++;
		count--;
	}
}
//---------------------------------------------------------------------------
//	lcdHome(..) - Cursor auf Position 1,1
//---------------------------------------------------------------------------
void lcdHome()
{
	lcdCmd(0x02);
	waitMs(2);			// warten
}
//---------------------------------------------------------------------------
//	lcdClear(..) - löscht die Anzeige im LCD
//---------------------------------------------------------------------------
void lcdClear()
{
	lcdCmd(0x01);
	waitMs(2);			// warten
}
//---------------------------------------------------------------------------
//	lcdOn(..) - schaltet das LCD an
//---------------------------------------------------------------------------
void lcdOn()
{
	lcdCmd(0x0E);
}
//---------------------------------------------------------------------------
//	lcdOff(..) - schaltet das LCD aus
//---------------------------------------------------------------------------
void lcdOff()
{
	lcdCmd(0x08);
}
//---------------------------------------------------------------------------
//	lcdGoto(..) - setzt die Cursorposition
// 	PE:	row = Zeile 1..2
//		col = Spalte 1..16
//---------------------------------------------------------------------------
void lcdGoto(int row, int col)
{
	row--;				// Null-basierend
	row &= 0x01;			// sicherheitshalber
	row *= 0x40;			// Zeile nach Bit 6 bringen
	col--;				// Null-basierend
	col &= 0x0f;			// sicherheitshalber
	char tmp = row | col;
	tmp |= 0x80;			// Cursor setzen
	lcdCmd(tmp);		// senden
}
//---------------------------------------------------------------------------
//	lcdInit(..) - Schaltet die Ports und Initialisiert das LCD
//---------------------------------------------------------------------------
void lcdInit()
{
	// Port D = Ausgang
	DDRD = 0xff;
	PORTD = 0;
	// warten bist LCD-Controller gebootet
	waitMs(50);
	// SOFT-RESET
	PORTD = 0x30;	//0b00110000;
	sbi(PORTD, 3);
	cbi(PORTD, 3);
	waitMs(5);
	PORTD = 0x30;	//0b00110000;
	sbi(PORTD, 3);
	cbi(PORTD, 3);
	waitMs(1);
	PORTD = 0x30;	//0b00110000;
	sbi(PORTD, 3);
	cbi(PORTD, 3);
	waitMs(5);

	// 4-BitModus einschalten
	PORTD = 0x20;
	// Schreibsignal
	sbi(PORTD, 3);
	cbi(PORTD, 3);
	waitMs(5);

	// ab hier im 4-Bit-Modus
	lcdCmd(0x28);		// Funktions-Set: 2 Zeilen, 5x7 Matrix, 4 Bit
	//lcdOff();
	lcdCmd(0x06);		// Entry Mode
	lcdOn();
	lcdClear();
}

long convertCharToSystem(char *c, int len, int base)
{
	long l = 0;
	//
	//101110
	for (int i = len-1; i >= 0; i--)
	{
		int i2 = c[i] - (int)'0';
		if (i2 > 9) i2 = c[i] - (int)'A' + 10;
		l += pow(base, i)*i2;
	}
	return l;
}
char* convert_bin(long i)
{
	int cnt = 0;
	long icopy = i;
	while (icopy != 0) 
	{
		cnt++;
		icopy >>= 1;
	}
	char *c = (char*)malloc(cnt+1);
	int ind = cnt-1;
	while (i != 0)
	{
		c[ind--] = i & 1 ? '1' : '0';
		i >>= 1;
	}
	c[cnt] = 0x00;
	return c;
}
char* convert_hex(long i)
{
	int cnt = 0;
	long icopy = i;
	while (icopy != 0)
	{
		cnt++;
		icopy >>= 4;
	}
	char *c = (char*)malloc(cnt+1);
	int ind = cnt-1;
	while (i != 0)
	{
		int h = i & 0b1111;
		if (h > 9) h = (int)'A' + (h - (int)'0' - 10);
		c[ind--] = h + (int)'0';
		i >>= 4;
	}
	c[cnt] = 0x00;
	return c;
}
char* convert_dez(long i)
{
	int cnt = 0;
	long icopy = i;
	while (icopy != 0)
	{
		cnt++;
		icopy /= 10;
	}
	char *c = (char*)malloc(cnt + 1);
	int ind = cnt - 1;
	while (i != 0)
	{
		c[ind--] = (i % 10) + (int)'0';
		i /= 10;
	}
	c[cnt] = 0x00;
	return c;
}

unsigned int readAdc()//unsigned char chanel=0)
{
	unsigned int AdcValue=0;
	ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);    // Frequenzvorteiler
	ADMUX = 0xC0;									 //Chanel
	ADCSRA |= (1<<ADSC);              // ADC aktivieren
	while(!(ADCSRA & (1<<ADIF)));     // auf Abschluss der Konvertierung warten (ADIF-bit)
	ADCSRA &= ~(1<<ADEN);             // ADC deaktivieren (2)
	AdcValue = ADC;
	return AdcValue;
}

#define LED_R 0b00001
#define LED_Y 0b00010
#define LED_G 0b00100
#define PLOAD 0b00010

#define LOW_D 800
#define HIGH_D 2800
#define LOW_C 500
  int main(void)
{
	lcdInit();
	DDRB = 0xFF;
	DDRC = 0x00;

	while(1)
	{
		long val2 = readAdc();
		long val = (4670*val2)/1023;
		lcdGoto(2,1);
		char *c = convert_dez(val);
		lcdWrite(c);
		lcdWrite("mV     ");

		lcdGoto(1,1);
		for(long i = 0; i < 16; i++) lcdWrite(" ");
		lcdGoto(1,1);
		for(long i = 0; i < val2/64; i++) lcdWrite1(0xFF);
		free(c);

		if(val <= LOW_D) PORTB |= LED_R;
		else PORTB &= ~LED_R;
		if(val >= HIGH_D) PORTB |= LED_G;
		else PORTB &= ~LED_G;

		if(val <= LOW_C)
		{
			DDRC |= PLOAD;
			PORTC |= PLOAD;
			PORTB |= LED_Y;
		}
		if(val >= HIGH_C) 
		{
			DDRC &= ~PLOAD;
			PORTC &= ~PLOAD;
			PORTB &= ~LED_Y;
		}
	}

	return 1;
}