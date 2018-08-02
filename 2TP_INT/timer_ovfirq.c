#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>

volatile int c=0;

ISR(TIMER1_OVF_vect) {PORTE^=1<<PORTE6;}
ISR(TIMER0_OVF_vect) {c++; if (c==5) {PORTB^=1<<PORTB5;c=0;}}

void timer_init()
{TCCR1B = (1 <<CS11)|(1<<CS10); 
 TIMSK1=(1<<TOIE1);               // diode verte
 
 TCCR0B = (1 <<CS02)|(1<<CS00);
 TIMSK0=(1<<TOIE0) ;
 sei();
}

int main( void ) {
  DDRB |=1<<PORTB5;
  DDRE |=1<<PORTE6;
  PORTB |= 1<<PORTB5;
  PORTE &=~1<<PORTE6;

  USBCON=0; // desactivation interrupt sur USB
 
  timer_init();
  while(1) {}
}
