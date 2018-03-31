#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "usart.h"

#define DEF_FREQ_A 11370L
#define OCR1A_RELOAD (F_CPU/(2*DEF_FREQ_A))
#define DEF_FREQ_A 20760L
#define OCR1B_RELOAD (F_CPU/(2*DEF_FREQ_B))

volatile unsigned int reloadA;
volatile unsigned int reloadB;

/* Timer 1 output compare A Interrupt Service Routine*/
ISR(TIMER1_COMPA_vect)
{
    OCR1A = OCR1A + reloadA;
    PORTB ^= 0x01;
}
/* Timer 1 output compare B Interrupt Service Routine*/
ISR(TIMER1_COMPB_vect)
{
    OCR1B = OCR1B + reloadB;
    PORTD ^= 0x80;
}

int main (void)
{
	char buff[32];
	unsigned long newF;
    
    reloadA=OCR1A_RELOAD;
    reloadB=OCR1B_RELOAD;

	DDRB = 0b00000111;      // set OC1A bit (PB1 or pin 15) and PB0 for output
    DDRD|= 0b10000000;
	TCCR1A=0x50;          // enable output compare mode to toggle OC1A (pin15)and OC1B (pin16) on match
	TCCR1B |= _BV(CS10);  // set prescaler to Clock/1
	TIMSK1 |= 0b00000110;// unmask output compare match interrupt for register A and B
	
	sei(); // enable global interupt

	usart_init (); // configure the usart and baudrate
	
	printf("Square wave generator for ATmega328p\r\n");
	printf("By Jesus Calvino-Fraga (c) 2018\r\n");
	
	while(1)
    {
    	printf("Frequency: ");
    	usart_gets(buff, 31);
    	newF=atol(buff);

	    if(newF>111000L)
	    {
	       printf("\r\nWarning: The maximum frequency that can be generated is around 111000Hz.\r\n");
	       newF=111000L;
	    }
	    if(newF>0)
	    {
			reloadA=(F_CPU/(2L*newF));
		    printf("\r\nFrequency set to: %ld\r\n", F_CPU/(reload*2L));
        }
    }
}
