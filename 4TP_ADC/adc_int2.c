// http://www.avrfreaks.net/forum/tut-c-newbies-guide-avr-adc
#define F_CPU 16000000UL
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>  // _delay_ms
#include "affiche.h"
#include "VirtualSerial.h"

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;
extern FILE USBSerialStream;

#define N 256

#define liaison_USB

volatile int indice;
volatile short res[N];
volatile short dummy; 

ISR(ADC_vect) {
  if (indice < N) {res[indice]=ADC;indice++; ADCSRA |= (1<<ADSC);}
}

void adc_init()
{ADMUX |= (1 << REFS0); // Set ADC reference to AVCC
 ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // no MUX for ADC0
 ADCSRA |= (1 << ADIE); // Enable ADC Interrupt
}

int main (void)
{unsigned short p;
 char s[9];
 s[0]='0';s[1]='x';s[6]='\r';s[7]='\n';s[8]=0;
#ifdef liaison_USB
 SetupHardware();
 CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
 GlobalInterruptEnable();
#endif
 adc_init();
 sei();	

 DDRB |=1<<PORTB5;
 DDRE |=1<<PORTE6;
 PORTB |= 1<<PORTB5;
 PORTE &= ~1<<PORTE6;

 indice=0;
 while (1)
   {
    if (indice==N) {
#ifdef liaison_USB
        fputs("\r\n", &USBSerialStream);
        for (p=0;p<N;p++) {affiche(res[p],&s[2]); fputs(s, &USBSerialStream);}
#else
        PORTB^=1<<PORTB5;PORTE^=1<<PORTE6;
#endif
        indice=0;
       }
    _delay_ms(10); 
    s[0]='0'+(indice&0x0f);s[1]='.';s[2]=0; fputs(s, &USBSerialStream);
//  les 3 lignes ci-dessous pour accepter les signaux venant du PC 
    CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    USB_USBTask();
   }
}

