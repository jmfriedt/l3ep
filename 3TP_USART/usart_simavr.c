#include <avr/io.h>      // voir /usr/lib/avr/include/avr/iom36u4.h
#define F_CPU 16000000UL
#include <util/delay.h>  // _delay_ms
#include <stdio.h>  // _delay_ms

///////////////////// start simavr
#include "avr_mcu_section.h"
AVR_MCU(F_CPU, "atmega32");
AVR_MCU_VCD_FILE("trace_file.vcd", 1000);
const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
        { AVR_MCU_VCD_SYMBOL("PORTB"), .what = (void*)&PORTB, },
        { AVR_MCU_VCD_SYMBOL("UDR1"), .what = (void*)&UDR1, },
};
///////////////////// end of simavr

#define USART_BAUDRATE (57600)

// https://github.com/tomvdb/avr_arduino_leonardo/blob/master/examples/uart/main.c
/*
void uart_transmit( unsigned char data )
{UDR1 = data;
 while ((UCSR1A& (1<<TXC1) )==0) ;
// UCSR1A|=(1<<TXC1);
}
*/

// function to send data
void uart_transmit (unsigned char data)
{while (!( UCSR1A & (1<<UDRE1)));                // wait while register is free
 UDR1 = data;                                   // load data in the register
}

unsigned char uart_receive(void)
{while (!(UCSR1A&(1<<RXC1))) ;
 return UDR1;
}

void send_byte(char c)
{char tmp;
 tmp=c>>4;     if (tmp<10) uart_transmit(tmp+'0'); else uart_transmit(tmp+'A'-10);
 tmp=(c&0x0f); if (tmp<10) uart_transmit(tmp+'0'); else uart_transmit(tmp+'A'-10);
}

void send_short(short s)
{send_byte(s>>8);
 send_byte(s&0xff);
}

void send_string(char *c)
{while (*c!=0) 
       {uart_transmit(*c);c++;}
}

int main(void){
unsigned short baud;
char c=42;  // '*'
char buffer[15]="Hello World\0"; // init statique ne marche pas dans simavr ?!
short s=0x1234;
  DDRB |=1<<PORTB5;
  DDRE |=1<<PORTE6;
  PORTB |= 1<<PORTB5;
  PORTE &= ~1<<PORTE6;

  UCSR1A = (1<<UDRE1); // 0;                         // importantly U2X1 = 0
  UCSR1B = (1 << RXEN1)|(1 << TXEN1); // enable receiver and transmitter
  UCSR1C = (1<<UCSZ11)|(1<<UCSZ10); // _BV(UCSZ11) | _BV(UCSZ10); // 8N1
  // UCSR1D = 0;                         // no rtc/cts (probleme de version de libavr)
  baud  = (((( F_CPU / ( USART_BAUDRATE * 16UL))) - 1));
  UBRR1H = (unsigned char)(baud>>8);
  UBRR1L = (unsigned char)baud;

// QUESTION : pourquoi allocation a l'init du tableau ne marche pas ?! Il *faut* remplir a la main
  buffer[0]='H'; buffer[1]='e'; buffer[2]='l'; buffer[3]='l'; buffer[4]='o'; buffer[5]=' ';
  buffer[6]='W'; buffer[7]='o'; buffer[8]='r'; buffer[9]='l'; buffer[10]='d'; buffer[11]=0;
  send_string(buffer);
  PORTB^=1<<PORTB5;
  send_short(s);          // affiche le message
  while (1){
    PORTB^=1<<PORTB5; // PORTE^=1<<PORTE6; // LED
    uart_transmit(c);                  // affiche le message
    uart_transmit(c+1);                  // affiche le message
    c+=2; if (c==127) c=32;
    _delay_ms(100);  
  }
  return 0;
}
