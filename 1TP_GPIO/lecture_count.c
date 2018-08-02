#include <avr/io.h> //E/S ex PORTB
#include <stdio.h>  //sprintf
#define F_CPU 16000000UL //T=62.5ns
#include <util/delay.h>  // _delay_ms
#include "VirtualSerial.h"



#define MAXCAR  25

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;
extern FILE USBSerialStream;


int main(void){
  int val=0,ancien=0,cpt=0;
  char aff[3];
  SetupHardware();
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
  GlobalInterruptEnable();

  DDRF  &=(~1<<PORTF0);// entree
  PORTF |=  1<<PORTF0; // pull up
  MCUCR &=~(1<<PUD);
  while(1) {val=(PINF&0x01);
            if (val!=ancien) 
               {cpt++;
                aff[0]='0'+(cpt/10);aff[1]='0'+(cpt%10);aff[2]=0;
                fputs("F=\0", &USBSerialStream);
                fputs(aff, &USBSerialStream);
                fputs("\r\n\0", &USBSerialStream);
               }
            ancien=val;
            CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
            CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
            USB_USBTask();
           }
}
