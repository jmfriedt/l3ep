#include <avr/io.h>

void adc_init()
{DIDR0=0xff;
 ADMUX = (1<<REFS0);        // AREF = AVcc, 2.5 V si (1<<REFS0|1<<REFS1)
 ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

unsigned short adc_read(uint8_t ch)
{ch &= 0x07;                // ch\in[0..7]
 ADMUX=(ADMUX & 0xF8)|ch;   // clears the bottom 3 bits before ORing
 ADCSRA |= (1<<ADSC);       // start single convertion
 while(ADCSRA & (1<<ADSC)); // poll jusqu'a fin de conversion
 return (ADC);
}
