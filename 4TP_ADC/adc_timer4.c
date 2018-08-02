// http://www.avrfreaks.net/forum/atmega32u4-starting-adc-conversion-timer4
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

EMPTY_INTERRUPT(TIMER4_OVF_vect);

volatile unsigned short adc,flag;

ISR(ADC_vect)
{adc = ADC;
 flag = 1;
}

void adc_init()
{ ADMUX = (1<<REFS0);   // quelle tension de reference ? quelle canal ?
  ADCSRB = (1<<ADTS3); // quelle source de declenchement de la mesure ?
  ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADATE) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}

int main(void)
{ char s[9];
  s[0]='0';s[1]='x'; s[6]='\r'; s[7]='\n'; s[8]=0;
  
  SetupHardware();
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
  GlobalInterruptEnable();
 
  TCCR4B = (1<<CS43) | (1<<CS42) | (1<<CS41) | (1<<CS40); // quel prescaler ?
  TIMSK4 = (1<<TOIE4); // quel taux de raffraichissement de l'interruption timer ?
  TC4H = 3; OCR4C = 0xFF; // Set TOP (overflow limit) to 0x3FF (= 1023)
 
  adc_init();
  sei();

  while (1)
   {
    if (flag) 
      {flag = 0;
       affiche(adc,&s[2]); fputs(s, &USBSerialStream);
//  les 3 lignes ci-dessous pour accepter les signaux venant du PC 
       CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
       CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
       USB_USBTask();
      }
   }
}
