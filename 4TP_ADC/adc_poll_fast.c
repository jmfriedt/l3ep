// TODO : potentiometre entre GND/3.3V pour les extremes et ADC0=A5
//http://maxembedded.com/2011/06/20/the-adc-of-the-avr/

#include <avr/io.h> //E/S ex PORTB 
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>  // _delay_ms
#include "affiche.h"
#include "VirtualSerial.h"

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;
extern FILE USBSerialStream;

void adc_init()
{
    ADMUX = (1<<REFS0); // AREF = AVcc
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
  unsigned short res[256];
  int k;
  char s[9];
  SetupHardware();
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
  GlobalInterruptEnable();

  DDRB |=1<<PORTB5;
  DDRE |=1<<PORTE6;
  PORTB |= 1<<PORTB5;
  PORTE &= ~1<<PORTE6;
  
  adc_init();
  sei();
  s[0]='0';s[1]='x';s[6]='\r';s[7]='\n';s[8]=0;

  while (1){
    PORTB^=1<<PORTB5;PORTE^=1<<PORTE6;
    _delay_ms(100); 
    for (k=0;k<256;k++) res[k]=adc_read(1);
    for (k=0;k<256;k++) {affiche(res[k],&s[2]); fputs(s, &USBSerialStream);}
//  les 3 lignes ci-dessous pour accepter les signaux venant du PC 
    CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    USB_USBTask();
  }
  return 0;
}
