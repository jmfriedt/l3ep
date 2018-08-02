#include <avr/io.h>
#define F_CPU 16000000UL

#define delai_leds (16000*2)
  
void timer1_init()  // Timer1 avec prescaler=64 et CTC (WGM=2)
{TCCR1B |= (1 << WGM12)|(1 << CS11)|(1 << CS10);
 OCR1A = delai_leds;  // valeur seuil <- delai
}
  
int main(void)
{
  DDRB |=1<<PORTB5;
  DDRE |=1<<PORTE6;
  PORTB |= 1<<PORTB5;
  PORTE &= ~(1<<PORTE6);
  
  timer1_init();
  while(1)
    {if (TIFR1 & (1 << OCF1A)) 
       {PORTB^=1<<PORTB5;PORTE^=1<<PORTE6;
        TIFR1 |= (1 << OCF1A); // clear flag
       }
    }
}



