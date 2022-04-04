/*
 * test.c
 *
 * Created: 10/26/2021 4:44:08 PM
 * Author : Yiang Gong
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <math.h>
#include <string.h>
#include <avr/interrupt.h>
#include "myUART.h"
#include "ST7735.h"
#include "LCD_GFX.h"
#include "tempSensor.h"

#define FREQ 16000000
#define BAUD 9600char str[25];

volatile int targetTime = 60;//mins
volatile int sec = 60;//secs

//on each timer 1 overflow, turn off led and stop buzzer
ISR(TIMER1_OVF_vect){
	sec--;
	if(sec==0){
		sec=60;
		targetTime--;
	}
	set(TIFR1,TOV1);//clear overflow flag
}

void timer1_init(){
	cli();
	//timer 1 setup, using prescaler 256
	set(TCCR1B,CS12);
	clear(TCCR1B,CS11);
	clear(TCCR1B,CS10);

	//set timer1 to mode 14
	set(TCCR1B,WGM13);
	set(TCCR1B,WGM12);
	set(TCCR1A,WGM11);
	clear(TCCR1A,WGM10);
	
	ICR1 = 62500;

	set(TIMSK1,TOIE1);//enable T1 overflow interrupt
	set(TIFR1,TOV1);//clear overflow flag

	sei();
}

void ADC_init(){
	//clear ADC power reduction
	clear(PRR,PRADC);
	//Vref = AVcc
	clear(ADMUX, REFS1);
	set(ADMUX,REFS0);
	//ADC prescaler 128
	set(ADCSRA, ADPS2);
	set(ADCSRA, ADPS1);
	set(ADCSRA, ADPS0);
	//using channel 0
	clear(ADMUX,MUX3);
	clear(ADMUX,MUX2);
	clear(ADMUX,MUX1);
	clear(ADMUX,MUX0);
	//free running mode
	set(ADCSRA,ADATE);//auto triggering
	clear(ADCSRB, ADTS2);
	clear(ADCSRB, ADTS1);
	clear(ADCSRB, ADTS0);
	//disable input buffer
	set(DIDR0,ADC0D);
	//enable ADC
	set(ADCSRA, ADEN);
	//start conversion
	set(ADCSRA, ADSC);
}

//read the adc value
int getADC(){
	while( !(ADCSRA & (1<<ADIF)) ){}
	int value = ADC;
	set(ADCSRA,ADIF);
	return value;
}

//this function maps value linearly to a different range
int my_mapping(int output_low, int output_high, int input_low, int input_high, int value){
	int input_difference = input_high - input_low;
	int output_difference = output_high - output_low;
	int output = (float)value/input_difference*output_difference + output_low;
	return output;
}

void Initialize(){
	lcd_init();
	UART_init();
	ADC_init();//adc0
	timer1_init();
	
	set(DDRD,4);//output for relay
	
	LCD_setScreen(0);
	LCD_drawString(5,5,"TARGET TEMPERATURE:", WHITE, BLACK);
	LCD_drawString(5,15,"CURRENT TEMPERATURE:", WHITE, BLACK);
	LCD_drawNum(130, 5, 0, WHITE, BLACK);
	LCD_drawString(5,65,"INPUT DEVICE:", WHITE, BLACK);
	LCD_drawString(5,85,"REMAINING TIME:", WHITE, BLACK);
}

int main(void){
	Initialize();
	
	int targetTemp = 40;
	int currentTemp = 0;
	int oldADC = 0, temp;
	while(1){
		//using adc channel 1
		clear(ADMUX,MUX3);clear(ADMUX,MUX2);clear(ADMUX,MUX1);set(ADMUX,MUX0);
		_delay_ms(20);
		temp = my_mapping(0, 60, 0, 1023, getADC());
		
		if(oldADC!=temp){
			targetTime = temp;
			oldADC = targetTime;
		}
		LCD_drawNum(5,95,targetTime, WHITE, BLACK);
		LCD_drawString(18,95," : ", WHITE, BLACK);
		LCD_drawNum(35,95,sec, WHITE, BLACK);
		if(targetTime<10&&targetTime>=0){
			LCD_drawString(11,95," ", WHITE, BLACK);
		}
		if(sec<10){
			LCD_drawString(41,95," ", WHITE, BLACK);
		}
		
		if(~PIND&(1<<3)){//if PD3 is low, use pot to adjust target temp
			//using adc channel 0
			clear(ADMUX,MUX3);clear(ADMUX,MUX2);clear(ADMUX,MUX1);clear(ADMUX,MUX0);
			_delay_ms(20);
			targetTemp = my_mapping(20, 80, 0, 1023, getADC());
			LCD_drawString(85,65,"POT  ", WHITE, BLACK);
		}else{//if PD3 is high, use blynk to adjust target temp
			LCD_drawString(85,65,"BLYNK", WHITE, BLACK);
			if(PIND&(1<<5)){
				targetTemp -=1;
			}else if(PIND&(1<<7)){
				targetTemp +=1;
			}
		}
		LCD_drawNum(130, 5, targetTemp, WHITE, BLACK);
		
		therm_read_temperature(&str);
		LCD_drawString(5,25,str, WHITE, BLACK);
		
		//logic control the relay
		if(targetTime>=0){
			currentTemp = atoi(str);
			if(currentTemp<targetTemp){
				set(PORTD,4);
			}else{
				clear(PORTD,4);
			}
		}else{
			clear(PORTD,4);
		}
		
		
	}
	
}
