/*	Author: jxie031
 *  Partner(s) Name: Xichao Wang
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

// Timer

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
/*
void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}*/
void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 100;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = 0;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr++;
	if(_avr_timer_cntcurr == _avr_timer_M){
		TimerISR();
		_avr_timer_cntcurr = 0;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = 0;
}
unsigned char counter = 0;
// State Machine

enum States {INIT, A0press, A1press, A0remain, A1remain, A0release, A1release, reset} state;

void TickFct_Latch(){
	unsigned char PB = PORTC;
	unsigned char flag = 0;
	unsigned char A1 = ~PINA & 0x02;
	unsigned char A0 = ~PINA & 0x01;
	/*
		if(A1 || A0) {
			while(!TimerFlag) {
				if (!(~PINA & 0x02)) {
					A1 = 0;
					break;
				}
				if (!(~PINA & 0x01)) {
					A0 = 0;
					break;
				}
			}
			TimerFlag = 0;
		}
		*/
	if (A0 && A1) state = reset;
	else {
		switch(state){
		 case INIT:
			if(A0) state = A0press;
			else if(A1) state = A1press;
			else state = INIT;
			break;
		case A0press:
			counter = 0;
			if(A0) state = A0remain;
			else if(!A0) state = A0release;
			else state = A0remain; 
			break;
		case A1press:
			counter = 0;
			if(A1) state = A1remain;
			else if(!A1) state = A1release;
			else state = A1remain;
			break;
		case A0remain:
			if(A0){
				state = A0remain;
				
			}else state = A0release;
			break;
		case A1remain:
			if(A1) state = A1remain;
			else state = A1release;
			break;
		case A0release:
			counter = 0;
			if(A0) state = A0press;
			else if(A1) state = A1press;
			else state = A0release;
			break;
		case A1release:
			counter = 0;
			if(A0) state = A0press;
			else if(A1) state = A1press;
			else state = A1release;
			break;
		case reset:
			if(A0) state = A0press;
			else if(A1) state = A1press;
			else state = reset;
			break;
		default:
			state = INIT;
			break; 
		}
	}
	switch(state){
	case INIT:
		PORTC = 7;
		break;
	case A0press:
		if(PB < 9)
			PORTC = PB + 1;
		break;
	case A0remain:
		counter++;
		if(counter == 100 && PB < 9)
		{
			PORTC = PB+1;
			counter = 0;
		}
		break;
	case A1press:
		if(PB > 0)
			PORTC = PB -1;
		break;
	case A1remain:
		counter++;
		if(counter == 100 && PB > 0)
		{
			PORTC = PB-1;
			counter = 0;
		}
		break;
	case reset:
		PORTC = 0;
		break;
	default:
		break;
	}
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs			
	state = INIT;
	TimerSet(1);
	TimerOn();
	// Initialize output on PORTB to 0x00
	while(1) {
		TickFct_Latch();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
	return 1;
}
