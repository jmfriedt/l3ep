// potentiometre entre GND/3.3V pour les extremes et ADC0=A5
//   ou sonde de temperature de la carte 7-segments (LM235)
//http://maxembedded.com/2011/06/20/the-adc-of-the-avr/

#include <avr/io.h> //E/S ex PORTB 
#define F_CPU 16000000UL
#include <util/delay.h>  // _delay_ms
#include "affiche.h"
#include "VirtualSerial.h"

//#define ascii

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;
extern FILE USBSerialStream;

void adc_init()
{   DIDR0=0xff; // http://www.openmusiclabs.com/learning/digital/atmega-adc
    ADMUX = (1<<REFS0); // AREF = AVcc, 2.5 V si (1<<REFS0|1<<REFS1)
    // ADC Enable and prescaler of 128 : 16 MHz/128=125 kHz
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

unsigned short adc_read(uint8_t ch)
{
  ch &= 0x07;              // ch\in[0..7]
  ADMUX=(ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
 
  ADCSRA |= (1<<ADSC);       // start single convertion
  while(ADCSRA & (1<<ADSC)); // poll jusqu'a fin de conversion
  return (ADC);
}

int main(void){
  short res=0;
  int minval[4],adcnum;
  char s[25],cmd;
  SetupHardware();
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
  GlobalInterruptEnable();

  DDRB |=1<<PORTB5;
  DDRE |=1<<PORTE6;
  PORTB |= 1<<PORTB5;
  PORTE &= ~1<<PORTE6;
  
minval[0]=0xCC;
minval[1]=0xA3;
minval[2]=0xB2;
minval[3]=0x90;
  adc_init();
  cmd=0x50;
  s[0]=0xff;s[1]=cmd;s[2]=cmd;s[3]=cmd;s[4]=cmd;

  while (1){
   PORTB^=1<<PORTB5;PORTE^=1<<PORTE6;
   _delay_ms(50); 
   for (adcnum=0;adcnum<4;adcnum++)
    {if (adcnum<2) res=adc_read(adcnum)-minval[adcnum];
        else res=adc_read(adcnum+2)-minval[adcnum];
     if (res<0) res=0;     // saturate BEFORE shifting
     res=res+(res>>1);     // * 1.5
     res=res>>2;           // /4
     if (res>=255) res=254; // threshold
#ifdef ascii
     affiche(res,&s[5*(adcnum)]); 
     s[5*(adcnum)+4]=' ';
#else
     s[adcnum+1]=(res&0xff);
#endif
    }

#ifdef ascii
    s[20]='\r';
    s[21]='\n';
    fwrite(s,1,22, &USBSerialStream);
#else
    fwrite(s,1,5, &USBSerialStream);
#endif
//  les 3 lignes ci-dessous pour accepter les signaux venant du PC 
    CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    USB_USBTask();
  }
  return 0;
}
