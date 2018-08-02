#include <avr/io.h> //E/S ex PORTB 
#define F_CPU 16000000UL

void mon_delai(long duree)
{long k=0;
 for (k=0;k<duree;k++) {};
}

int main(void){
  DDRB |=1<<PORTB5;
  DDRE |=1<<PORTE6;
  PORTB |= 1<<PORTB5;
  PORTE &= ~(1<<PORTE6);

  while (1){
    PORTB^=1<<PORTB5;PORTE^=1<<PORTE6;
    mon_delai(0xffff);
  }
  return 0;
}
