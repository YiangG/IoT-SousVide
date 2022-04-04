//using baud rate 9600, and 2 stop bits
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include "myUART.h"
#include <avr/io.h>

void UART_init(){
  /*Set baud rate */
  UBRR0H = (unsigned char)(BAUD_PRESCALER>>8);
  UBRR0L = (unsigned char)BAUD_PRESCALER;
  //Enable receiver and transmitter 
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  /* Set frame format: 2 stop bits, 8 data bits */
  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // 8 data bits
  UCSR0C |= (1<<USBS0); // 2 stop bits
}

void UART_sendChar(unsigned char data){
  while(!(UCSR0A & (1<<UDRE0)));// Wait for empty transmit buffer
  UDR0 = data;// Put data into buffer and send data
  
}

void UART_sendString(char* StringPtr){
  while(*StringPtr != 0x00){
    UART_sendChar(*StringPtr);
    StringPtr++;
  }
}
