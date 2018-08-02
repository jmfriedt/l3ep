#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

void init(){
cli();
DDRB |=1<<PORTB5;
  DDRE |=1<<PORTE6;
  PORTE |= 1<<PORTE6;
  PORTB &= ~1<<PORTB5;
TIMSK0=0;TIMSK1=0;  TIMSK2=0;  TIMSK3=0;  TIMSK4=0;  TIMSK5=0;
  TCCR0A=0;
  TCCR0B=3;
//prescaler /256 pour le timer 1 et mode CTC
 //TCNT1 = 0;
 //TCNT0 = 0;
 OCR1A = 255;  // valeur seuil <- delai
 // TIMSK1 |= (1 << OCIE1A);
    TIMSK1|=(1<<OCIE1A);
    TIMSK0|=(1<<TOIE0);


    TCCR0B |= (1<<CS02) | (1<<CS00);
 TCCR1B |= (1 << WGM12)|(1 << CS12)|(1<< CS10);

    // lets turn on 16 bit timer1 also with /1024
//   TCCR1B |= (1 << CS10) | (1 << CS12);

wdt_disable();
    // enable interrupts
    sei();

}

ISR(TIMER1_COMPA_vect) {PORTB^=1<<PORTB5;}

ISR(TIMER0_OVF_vect) {PORTE^=1<<PORTE6;}

int main( void ) {
	init();
    while(1) {
    }
}
