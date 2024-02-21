#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "libi2c.h"

#include <string.h>
#include <stdio.h>

#include "VirtualSerial.h"

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;
extern FILE USBSerialStream;

int main(void){
  uint8_t i;
  uint8_t mask[]={0x7F, 0x7F, 0x3F, 0x3F, 0x07, 0x1F, 0xFF};
  uint8_t time[16];               // Le PCF8563 possede 16 registres
  char buffer[80]="C'est parti\r\n\0";
  SetupHardware();
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
  GlobalInterruptEnable();
  fputs(buffer, &USBSerialStream);
  I2C_UEXT(ON);                   // Alimentation du connecteur UEXT on (D8=PB4)
  I2C_init(24, 0);                // Initialisation du bus I2C : 24 pour 400kHz
  I2C_stop();

  time[0]=2;
  time[1]=0x00;time[2]=0x12;time[3]=5;time[4]=6;time[5]=7;time[6]=8; time[7]=9;
  // second            min         hour      day      weekday   month     year

  // initialisation RTC
  for (i=0;i<10;i++)  // repeat: "all accesses must be completed within 1 second" (p.12 DS)
    if (I2C_write(PCF8563,(uint8_t *)time,8)!=0)   // tester l'echec
      fputs("Echec ecriture\r\n\0", &USBSerialStream);
  while (1){
    time[0]=2;                            // registre qu'on veut lire
    I2C_rw(PCF8563, time, 1, time+1, 7);  // lecture de la RTC
    for(i=0; i<7; i++) 
       {time[i+1]=time[i+1]&mask[i];
        buffer[2*i+1]=(time[i+1]&0x0f)+'0';
        buffer[2*i]=((time[i+1]&0xf0)>>4)+'0';
       }
    buffer[14]='\r';buffer[15]='\n';buffer[16]=0; // affichage
    fputs(buffer, &USBSerialStream);
    CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    USB_USBTask();
    _delay_ms(1000);                      // attente pour laisser RTC tourner
  }
  return 1;
}
