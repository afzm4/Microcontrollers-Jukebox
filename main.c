#include <reg932.h>
#include "uart.h"

#define IE 0xA8

//Pin variables
sbit out = P1^1;
sbit speaker = P1^7;
sbit pin1 = P0^0;
sbit pin2 = P1^2;
sbit pin3 = P1^3;

//Switch variables
sbit sw1 = P2^0;
sbit sw2 = P0^1;
sbit sw3 = P2^3;
sbit sw4 = P0^2;
sbit sw5 = P1^4;
//sbit sw6 = P0^0;
sbit sw7 = P2^1;
sbit sw8 = P0^3;
sbit sw9 = P2^2;

//LED variables
sbit led1 = P2^4;
sbit led2 = P0^5;
sbit led3 = P2^7;
sbit led4 = P0^6;
sbit led5 = P1^6;
sbit led6 = P0^4;
sbit led7 = P2^5;
sbit led8 = P0^7;
sbit led9 = P2^6;

//function definitions
void playtime(int freq, int time);
void playnote(const int freq);
void mode1();
void mode2();
void mode3();
void dream_land();
void melee();
void ledON();
void ledOFF();
void melee_theme();
void dreamland_theme();
void replay();

//global variable declarations
int mode = 0;
int LC = 0;
int w = 0;
int song[30];
int freq_table[7] = { 440, 494, 523, 587, 659, 698, 784 };

void main()
{
	//set to bi-directional;
	P0M1 = 0x00;
	P1M1 = 0x00;
	P2M1 = 0x00;
	P3M1 = 0x00;
	// main loop for program
	while(1)
	{
		if(mode == 0)
			mode1();
		else if(mode == 1)
			mode2();
		else if(mode == 2)
			mode3();
	}
}

//tune mode
void mode1()
{
	int i = 0;
	led1 = 0;
	led2 = 1;
	led3 = 1;
	if(!sw3) //switch modes
	{
		while(!sw3);
		for(i; i < 255; i++);
		mode = 1;
	}

	if(!sw4) //play tune1
	{	
		dream_land();
	}
	if(!sw5)  //play tune2
	{
		melee();
	}
}

//free play mode
void mode2()
{
	int i = 0;
	led1 = 1;
	led2 = 0;
	led3 = 1;
	if(!sw3)  //switch modes
	{
		while(!sw3);
		for(i; i < 255; i++);
		mode = 2;
	}

	if (!sw1)
	{
		replay();
	}
	//play A
	while(!sw7)
	{
		led7 = 0;
		playtime(440, 16);
		if(w<30)
		{
			song[w] = 440;  //adds note to song
			w++; //increments the replay counter
		}
	}
	led7 = 1;
	
	//play B
	while(!sw8)
	{
		led8 = 0;
		playtime(494, 16);
		if(w<30)
		{
			song[w] = 494;  //adds note to song
			w++;  //increments the replay counter
		}
	}
	led8 = 1;

	//play C
	while(!sw9)
	{
		led9 = 0;
		playtime(554, 16);
		if(w<30)
		{
			song[w] = 554;  //adds note to song
			w++;  //increments the replay counter
		}
	}
	led9 = 1;
}

// Thomas Priest: Play from serial input
void mode3()
{
	char cnote;		// store user inputs
	int inote;
	int i = 0;
	
	led1 = 1;	// set LEDs to display mode 3
	led2 = 1;
	led3 = 0;
	

	EA = 1;			// enable serial communication
	uart_init();
	while(RI == 0)		// Look for serial input
	{
			if(!sw3)  //switch modes
		{
			while(!sw3);
			for(i; i < 255; i++);
			mode = 0;
			EA = 0;
			return;
		}
	}
	
	cnote = uart_get();		// receive input
	RI = 0;
	
	inote = freq_table[(cnote - 65)];	// calculate frequency index and look up on table
	playtime(inote, 16);			// play note
	

	EA = 0;
}

//note playing function
void playtime(int freq, int time)
{
	int i = 0;
	EA = 1;
	uart_init();
	uart_transmit((freq / 100 % 10)+ '0');
	uart_transmit((freq / 10 % 10) + '0');
	uart_transmit((freq % 10) + '0');
	uart_transmit('H');
	uart_transmit('z');
	uart_transmit('\r');
	uart_transmit('\n');
	EA = 0;
	if(freq != 0) //UART display
		ledON();
	TMOD = 0x11;  //set timers
	for (i; i < time; i++)
	{
		TH0 = -36865 >> 8;  //shift values 8
		TL0 = -36865;
		TR0 = 1;  //start timer
		while(!TF0)
		{
			playnote(freq);  
		}
		TF0 = 0;  //turn off timer
	}
	if(freq != 0)
		ledOFF();
}

void playnote(const int freq)
{
	int n = (1843250 / freq)-500;  //calculate n value
	TMOD = 0x11;  //set timers
	if (freq > 20)
	{
		TH1 = -n >> 8;  //shift 8
		TL1 = -n;
		TR1 = 1;  //turn on timer
		while(!TF1);
		speaker = ~speaker;  //square wave
		TF1 = 0;  //turn off timer
	}
}

//send dreamland title via serial comms
void dreamland_theme()
{
	int i;
	EA = 1;
	uart_init();
	uart_transmit('D');
	uart_transmit('r');
	uart_transmit('e');
	uart_transmit('a');
	uart_transmit('m');
	uart_transmit('l');
	uart_transmit('a');
	uart_transmit('n');
	uart_transmit('d');
	uart_transmit(' ');
	uart_transmit('T');
	uart_transmit('h');
	uart_transmit('e');
	uart_transmit('m');
	uart_transmit('e');
	uart_transmit('\r');
	uart_transmit('\n');
	for(i = 0; i < 2000; i++);
	EA = 0;
}

//sned melee title via serial comms
void melee_theme()
{
	int i; 
	EA = 1;
	uart_init();
	uart_transmit('M');
	uart_transmit('e');
	uart_transmit('l');
	uart_transmit('e');
	uart_transmit('e');
	uart_transmit(' ');
	uart_transmit('T');
	uart_transmit('h');
	uart_transmit('e');
	uart_transmit('m');
	uart_transmit('e');
	uart_transmit('\r');
	uart_transmit('\n');
	for(i = 0; i < 2000; i++);
	EA = 0;
}

//play dream land theme 
void dream_land()
{
	dreamland_theme();
	playtime(1175, 32);
	playtime(0, 32);
	playtime(880, 32);
	playtime(0, 32);
	playtime(698, 32);
	playtime(659, 32);
	playtime(587, 32);
	playtime(0, 32);
	playtime(587, 32);
	playtime(659, 32);
	playtime(698, 32);
	playtime(587, 32);
	playtime(523, 32);
	playtime(587, 32);
	playtime(440, 32);
}

//play melee theme
void melee()
{
	melee_theme();
	playtime(493, 16);
	playtime(370, 8);
	playtime(494, 8);
	playtime(740, 48);
	playtime(494, 16);
	playtime(740, 16);
	playtime(880, 16);
	playtime(831, 16);
	playtime(659, 16);
	playtime(494, 64);
	playtime(0, 16);
	playtime(494, 8);
	playtime(554, 8);
	playtime(587, 80);
	playtime(494, 16);
	playtime(587, 16);
	playtime(740, 16);
	playtime(659, 128);
}

//turn on LEDS on the breadboard
void ledON()
{
	ES = 0;
	if(LC == 0)
		pin1 = 0;
	if(LC == 1)
		pin2 = 0;
	if(LC == 2)
		pin3 = 0;
}

//turn off LEDS on the breadboard
void ledOFF()
{
	if(LC == 0)
		pin1 = 1;
	if(LC == 1)
		pin2 = 1;
	if(LC == 2)
		pin3 = 1;
	LC++;
	if(LC == 3)
		LC = 0;
}

//Andrew Floyd Song Maker Function
void replay()
{
	int i = 0;
	int y = 0;
	for (i; i < w; i++)  //replays the song that is saved
	{
		playtime(song[i], 16);
	}
	for (y; y < 34; y++)  //clears the song
	{
		song[y] = 0;
	}
	w = 0;  //resets the song starting point
}
