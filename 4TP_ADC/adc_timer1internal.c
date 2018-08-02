// http://www.narkidae.com/research/atmega-core-temperature-sensor/
#include <avr/io.h> //E/S ex PORTB 
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <util/delay.h>  // _delay_ms
#include "affiche.h"
#include "VirtualSerial.h"

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;
extern FILE USBSerialStream;

EMPTY_INTERRUPT(TIMER1_OVF_vect);
EMPTY_INTERRUPT(TIMER1_COMPB_vect);

volatile unsigned short adc,flag;

ISR(ADC_vect)
{adc = ADC;    // temperature en Kelvin a +/- 10K pres
 flag = 1;
}

void adc_init() 
{ ADMUX  = (1<<REFS0)|(1<<REFS1)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0);   // 2.56V, 00111
  ADCSRB = (1<<ADTS2)|(1<<ADTS0)|(1<<MUX5); // T=100111 avec MUX5 dans SRB
  ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADATE) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

 //  ADMUX = (1<<REFS0)|5;          // reference AVCC, channel 5
 //  ADCSRA = (1<<ADEN)|(1<<ADATE)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);  
 //  ADCSRB = (1<<ADTS2)|(1<<ADTS0); //Setting compare match B as trigger source
}

int main(void)
{ char s[9];
  s[0]='0';s[1]='x';s[6]='\r';s[7]='\n';s[8]=0;
  
  SetupHardware();
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
  GlobalInterruptEnable();
  
  //TCCR1B = (1<<CS12) | (1<<CS10); // quel prescaler ? (PAS CS11 sinon external source)
  //TIMSK1 = (1<<TOIE1); // quel taux de raffraichissement de l'interruption timer ?
  //TCNT1H = 3; OCR1C = 0xFF; // Set TOP (overflow limit) to 0x3FF (= 1023)
                          // voir datasheet page 160 (T4OVF set by TOP defined by OCR4C -- Table 15-19
 
   TCCR1B = (1<<WGM12)|(1<<CS12); // Set  CTC with prescaler = 256 
   TCCR1A = (1<<COM1B0);          // OC1A pin in toggle mode
   TIMSK1 = (1<<TOIE1);
   OCR1A = 6250;                  // TOP = 200ms   
   OCR1B = 6250;                  // Initialize compare value

  adc_init();
  sei();

  while (1)
    {
        if (flag) {
            flag = 0;

if(bit_is_set(TIFR1,OCF1B)) {
   adc=ADC;

    affiche(adc,&s[2]); fputs(s, &USBSerialStream);
             TIFR1 =(1<<OCF1B);         //Manually clearing the OCF1B flag
}

//  les 3 lignes ci-dessous pour accepter les signaux venant du PC 
        }
    CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    USB_USBTask();
    }
}
