#include <avr/io.h>

void adc_init(uint8_t ref)
{DIDR0=0xff;
 ADMUX = (ref<<REFS0);        // AREF = AVcc, 2.5 V si (1<<REFS0|1<<REFS1)
 ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

unsigned short adc_read(uint8_t ch) // ch\in[0..63]
{if (ch>31) ADCSRB |= (1<<MUX5);
 else ADCSRB &= ~(1<<MUX5);
 ch &= 0x1F;                // ch\in[0..31]
 ADMUX=(ADMUX & 0xE0)|ch;   // clears the bottom 5 bits before ORing
 ADCSRA |= (1<<ADSC);       // start single convertion
 while(ADCSRA & (1<<ADSC)); // poll jusqu'a fin de conversion
 return (ADC);
}
