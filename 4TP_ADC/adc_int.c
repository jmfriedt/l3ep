// http://www.avrfreaks.net/forum/tut-c-newbies-guide-avr-adc
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>  // _delay_ms
#include "affiche.h"
#include "VirtualSerial.h"

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;
extern FILE USBSerialStream;

#define N 256

volatile char flag=0;

ISR(ADC_vect) {flag=1;}

void adc_init()
{ADMUX  = (1<<REFS0); // Set ADC reference to AVCC
 ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // no MUX for ADC0
 ADCSRA |= (1 << ADIE); // Enable ADC Interrupt
}

int main (void)
{volatile unsigned short res[N],indice=0;
 char s[9];
 int k;
 s[0]='0';s[1]='x';s[6]='\r';s[7]='\n';s[8]=0;

 SetupHardware();
 CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
 GlobalInterruptEnable();

 adc_init();
 sei();	
 ADMUX|=0x01; // ADC1
 ADCSRA |= (1<<ADSC);          // start single conversion
 while (1)
   {if (flag!=0) 
      {res[indice]=ADC;flag=0; // fin de conversion par interrupt
       indice++;
       if (indice==N) 
          {for (k=0;k<N;k++) 
             {affiche(res[k],&s[2]);fputs(s, &USBSerialStream);
// est-ce malin de mettre USB dans la boucle plutot que de remplir 
//   le descripteur de fichier et envoyer en une fois ?
              CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
              USB_USBTask(); //  communication avec le PC 
             }
           indice=0;
          }
       ADCSRA |= (1<<ADSC);  // start single conversion
      }
//  _delay_ms(100); 
   }
}
