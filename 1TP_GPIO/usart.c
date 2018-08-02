#include <avr/io.h>      // voir /usr/lib/avr/include/avr/iom32u4.h
#define F_CPU 16000000UL
#include <util/delay.h>  // _delay_ms

#define USART_BAUDRATE (57600)

// https://github.com/tomvdb/avr_arduino_leonardo/blob/master/examples/uart/main.c
void uart_transmit( unsigned char data )
{while (!(UCSR1A&(1<<UDRE1))) ;
 UDR1 = data;
}

unsigned char uart_receive(void)
{while (!(UCSR1A&(1<<RXC1))) ;
 return UDR1;
}

int main(void){
unsigned short baud;
char c=42;  // '*'
  DDRB |=1<<PORTB5;
  DDRE |=1<<PORTE6;
  PORTB |= 1<<PORTB5;
  PORTE &= ~1<<PORTE6;

  UCSR1A = 0;                         // importantly U2X1 = 0
  UCSR1B = 0;                         
  UCSR1B = (1 << RXEN1)|(1 << TXEN1); // enable receiver and transmitter
  UCSR1C = _BV(UCSZ11) | _BV(UCSZ10); // 8N1
  // UCSR1D = 0;                         // no rtc/cts (probleme de version de libavr)
  baud  = (((( F_CPU / ( USART_BAUDRATE * 16UL))) - 1));
  UBRR1H = (unsigned char)(baud>>8);
  UBRR1L = (unsigned char)baud;

  while (1){
    PORTB^=1<<PORTB5;PORTE^=1<<PORTE6; // LED
    _delay_ms(10);  

    uart_transmit(c);                  // affiche le message
    c++; if (c==127) c=32;
  }
  return 0;
}

