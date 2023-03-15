#include <avr/io.h>      // voir /usr/lib/avr/include/avr/iom32u4.h
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>  // _delay_ms

#define USART_BAUDRATE 9600

#ifdef atmega32u2
// pour simavr
#include <stdio.h>  
#include "avr_mcu_section.h"
AVR_MCU(F_CPU, "atmega32");

const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
                { AVR_MCU_VCD_SYMBOL("PORTB"), .what = (void*)&PORTB, },
        };
// fin simavr

#include <stdio.h>
static int uart_putchar(char c, FILE *stream) {
  if (c == '\n')
    uart_putchar('\r', stream);
  //loop_until_bit_is_set(UCSR0A, UDRE0);
  //UDR0 = c;
  return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,
                                         _FDEV_SETUP_WRITE);
#endif

volatile char received,flag=0;

ISR(USART1_RX_vect) {received=UDR1;flag=1;}

void uart_transmit( unsigned char data )
{while (!(UCSR1A&(1<<UDRE1))) ;
 UDR1 = data;
}

int main(void){
unsigned short baud;
  DDRB |=1<<PORTB5;    // LEDs
  PORTB |= 1<<PORTB5;
#ifndef atmega32u2
  DDRE |=1<<PORTE6;
  PORTE &= ~1<<PORTE6;
#endif

  UCSR1A = 0;                           // importantly U2X1 = 0
  UCSR1B = (1 << RXEN1) | (1 << TXEN1); // enable receiver and transmitter
  UCSR1B|= (1 << RXCIE1);               // ACTIVATION INTERRUPTION RX Completed
  UCSR1C = _BV(UCSZ11) | _BV(UCSZ10);   // no parity, 8 data bits, 1 stop bit
// UCSR1D = 0;                           // no cts, no rts
  baud  = (((( F_CPU / ( USART_BAUDRATE * 16UL))) - 1));
  UBRR1H = (unsigned char)(baud>>8);
  UBRR1L = (unsigned char)baud;

  USBCON=0; // desactive l'interruption USB
  sei();

#ifdef atmega32u2
  stdout=&mystdout;
#endif 

  while (1){
    PORTB^=1<<PORTB5; 
#ifndef atmega32u2
    PORTE^=1<<PORTE6;
#endif
    _delay_ms(100);  //Attente de 500ms

    if (flag!=0) {flag=0;uart_transmit(received+1);}
  }
  return 0;
}
