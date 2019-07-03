/*	Author: jxie031
 *  Partner(s) Name: Xichao Wang
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States {stay, inc, man} state;
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	//DDRB = 0x00; PORTB = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	unsigned char A;
	state = stay;
	unsigned char B = 0x00;
    /* Insert your solution below */
    while (1) {
		A = ~PINA;
		switch(state){
			case stay: // A= 0; B= B
				if(A == 1)	state = inc;
				else state = stay;
				break;
			case inc:// A= 1; B++
				if(A == 1) state = man;
				else state = stay;
				break;
			case man:// A= 1; B= B
				if(A == 1) state = man;
				else state = stay;
				break;
			default:
				state = stay;
				break;
		}
		switch(state){
			case stay:
				break;
			case inc:
				if(B == 0 || B == 32)
					B = 1;
				else 
				B = B << 1;
				break;
			case man:
				break;
			default:
				break;
		}
				
		PORTB = B;
    }
    return 1;
}
