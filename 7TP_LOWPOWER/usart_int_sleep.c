#include"libttycom.h"
#include <avr/io.h>      // voir /usr/lib/avr/include/avr/iom32u4.h
#include <avr/interrupt.h>
#include <avr/sleep.h>
#define F_CPU 16000000UL
#include <util/delay.h>  // _delay_ms

#define USART_BAUDRATE 9600
volatile char flag=0;

ISR(INT2_vect) {flag=1;}

void uart_transmit( unsigned char data )
{while (!(UCSR1A&(1<<UDRE1))) ;
 UDR1 = data;
}

void uart_puts(char *s)
{int k=0;
 while (s[k]!=0) {uart_transmit(s[k]);k++;}
}

int main(void){
  unsigned short baud;
  char s[8]="hello\r\n\0";
  init_olimexIno();
  DDRB |=1<<PORTB5;    // LEDs
  DDRE |=1<<PORTE6;
  PORTB |= 1<<PORTB5;
  PORTE &= ~1<<PORTE6;

  UCSR1A = 0;                           // importantly U2X1 = 0
  UCSR1B = (1 << RXEN1) | (1 << TXEN1); // enable receiver and transmitter
  UCSR1C = _BV(UCSZ11) | _BV(UCSZ10);   // no parity, 8 data bits, 1 stop bit
  UCSR1D = 0;                           // no cts, no rts
  baud  = (((( F_CPU / ( USART_BAUDRATE * 16UL))) - 1));
  UBRR1H = (unsigned char)(baud>>8);
  UBRR1L = (unsigned char)baud;

  EIMSK = 1<<INT2;	//enable int2
  sei();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  while (1){
    uart_puts(s);
    _delay_ms(100); 
    if (flag!=0) {flag=0;PORTB^=1<<PORTB5;}	 // pourquoi ca ne fait rien ?
    PORTE^=1<<PORTE6;
    sleep_mode();
    // sleep_disable();  // premiere chose a faire quand on se reveille -- include dans _mode
  }
  return 0;
}
