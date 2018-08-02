#include <avr/io.h> //E/S ex PORTB 
#define F_CPU 16000000UL
#include <util/delay.h>  // _delay_ms

# define USART_BAUDRATE 9600

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
char c=32;
  DDRB |=1<<PORTB5;
  DDRE |=1<<PORTE6;
  PORTB |= 1<<PORTB5;
  PORTE &= ~1<<PORTE6;

  UCSR1A = 0;                         // importantly U2X1 = 0
  UCSR1B = 0;                         // interrupts enabled in here if you like
  UCSR1B = (1 << RXEN1)|(1 << TXEN1); // enable receiver and transmitter
  UCSR1C = _BV(UCSZ11) | _BV(UCSZ10); // 8N1
  UCSR1D = 0;                         // no rtc/cts
  baud  = (((( F_CPU / ( USART_BAUDRATE * 16UL))) - 1));
  UBRR1H = (unsigned char)(baud>>8);
  UBRR1L = (unsigned char)baud;

  while (1){
    c=uart_receive(); 
    uart_transmit(c+1);
  }
  return 0;
}

