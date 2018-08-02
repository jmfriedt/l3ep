#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define F_CPU 16000000UL

#define delai_leds (16000*2)

#define avec_interruption

ISR (TIMER1_COMPA_vect) {PORTB^=1<<PORTB5;PORTE^=1<<PORTE6;}

void timer1_init()  // Timer1 avec prescaler=64 et CTC
{
 TCCR1A=0;
 TCCR1B= (1 << WGM12)|(1 << CS11)|(1 << CS10);
 TCNT1=0;
 OCR1A = delai_leds;      // valeur seuil <- delai
#ifdef avec_interruption
 TIMSK1= (1 << OCIE1A);   // active interruption
 sei();
#endif
}

int main(void)
{
#ifdef avec_interruption
  USBCON=0;   // https://github.com/pololu/a-star/issues/3
#endif

  DDRB |=1<<PORTB5;
  DDRE |=1<<PORTE6;
  PORTB |= 1<<PORTB5;
  PORTE &= ~(1<<PORTE6);

  timer1_init();
  while(1) {
#ifndef avec_interruption
       if (TIFR1 & (1 << OCF1A)) 
          {PORTB^=1<<PORTB5;PORTE^=1<<PORTE6;}
       TIFR1 |= (1 << OCF1A);
#endif
  }
}
