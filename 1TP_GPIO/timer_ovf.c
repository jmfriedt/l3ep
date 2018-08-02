#include <avr/io.h>
#define F_CPU 16000000UL

void timer1_init()  // Timer1 avec prescaler=64 et mode normal
{TCCR1B |= (1 << CS11)|(1 << CS10);
}
  
int main(void)
{
  DDRB |=1<<PORTB5;
  DDRE |=1<<PORTE6;
  PORTB |= 1<<PORTB5;
  PORTE &= ~(1<<PORTE6);
  
  timer1_init();
  while(1)
    {if (TIFR1 & (1 << TOV1)) 
       {PORTB^=1<<PORTB5;PORTE^=1<<PORTE6;
        TIFR1 |= (1 << TOV1); // clear flag
       }
    }
}



