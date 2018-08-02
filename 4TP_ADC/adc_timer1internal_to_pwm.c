// test avec Tektro AFG3102, period=1 s, sine ampli=500 mVpp, offset=250 mV
// sortie PWM D6
// ADC A5
// timing sur LED2/D9 = PB5 <= vitesse d'acquisition

// http://www.narkidae.com/research/atmega-core-temperature-sensor/
#include <avr/io.h> //E/S ex PORTB 
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <util/delay.h>  // _delay_ms

EMPTY_INTERRUPT(TIMER1_OVF_vect);
EMPTY_INTERRUPT(TIMER1_COMPB_vect);

volatile unsigned short adc,flag=0;

void pwm_configure()
{TCCR4A=0;
 TCCR4B=(1<<CS40);        // rapide
 TCCR4C=0;
 TCCR4D=0;
 PLLFRQ=(PLLFRQ&0xCF)|0x30; // PLL_FREQ 48000000UL
 TC4H=0x00;                 // compteur sur 10 bits (hi)
 OCR4C=127;                 // Terminal count for Timer 4 PWM: val max
}

void pwmD_configure() // Timer4 D
{TCCR4C|=0x09; // Activate channel D    PD7 Atmega=D6 Arduino
 DDRD|=1<<7;   // Set Output Mode BIEN QUE PWM, DOIT ETRE OUTPUT
}

ISR(ADC_vect)
{adc = ADC;    // temperature en Kelvin a +/- 10K pres
 flag = 1;
// PORTB^=(1<<PORTB5);
}

void adc_init()  // 500 kHz clock + fast = 15.6 kS/s
{ ADMUX  = (1<<REFS0)|(1<<REFS1);   // 2.56V, ADC0
  ADCSRB = (1<<ADTS2)|(1<<ADTS0)|(1<<ADHSM);   // TS2-TS0 : compare match b de timer 1 + hi speed
  ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADATE) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS0);// 500 kHz
}                                 // automatic

int main(void)
{wdt_disable();
 DDRD|=(1<<PORTD3);
 DDRE|=(1<<PORTE6);
 DDRB|=((1<<PORTB5)|(1<<PORTB6));
  
 USBCON=0; // desactivation interrupt sur USB
 
 TCCR1B = (1<<WGM12)|(1<<CS11); // Set  CTC with prescaler = 1
 TCCR1A = (1<<COM1B0);          // OC1B pin in toggle mode
 TIMSK1 = ((1<<TOIE1)|(1<<OCIE1B));
 OCR1A  = 65;                  // TOP = 200ms   vitesse de conversion ADC
 OCR1B  = 65;                  // TOP = 200ms   
 
 pwm_configure();
 pwmD_configure();
 adc_init();
 sei();

 while (1)
   {if (flag) 
      {flag = 0;
       adc=ADC;
       TIFR1 =(1<<OCF1B);         //Manually clearing the OCF1B flag
       OCR4D=(adc>>2);            // mirror ADC to PWM : 10 bits -> 0..127 par >>3
       PORTB^=(1<<PORTB5);
      }
   }
}
