// probleme de conflit avec USB sur INT2 + probleme de memoire disponible => passer en uart

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "libi2c.h"

#include "VirtualSerial.h"

volatile static uint8_t flag=0;

ISR(INT2_vect) {flag=1;}

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;
extern FILE USBSerialStream;

int main(void){
  uint8_t i;
  uint8_t mask[]={0x7F, 0x7F, 0x3F, 0x3F, 0x07, 0x1F, 0xFF};
  uint8_t time[16];               // Le PCF8563 possede 16 registres
  uint8_t buffer[80]="C'est parti\r\n\0";
  SetupHardware();
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
  GlobalInterruptEnable();
  fputs(buffer, &USBSerialStream);

  I2C_UEXT(OFF);                  // reset RTC
  sei();
  I2C_UEXT(ON);                   // Alimentation du connecteur UEXT on (D8=PB4)
  I2C_init(24, 0);                // Initialisation du bus I2C : 24 pour 400kHz
  I2C_stop();

//  MCUCR &=~1<<PUD;
//  DDRD  &=~1<<PORTD2; //int2 sur PD2
//  PORTD |= 1<<PORTD2; //pullup on <- ne PAS activer

  time[0]=1;
  time[1]=(1<<1)+(1<<3);
  I2C_write(PCF8563,(uint8_t *)time,2);   // tester l'echec

  time[0]=2;
  time[1]=0x55;time[2]=2;time[3]=3;time[4]=4;time[5]=5;time[6]=6; time[7]=9; 
  // second    min         hour       day      weekday   month     year
  time[8]=0x03;time[9]=0x80;time[10]=0x80;time[11]=0x80;
  // minute alm    deactivate hr/day/weekday alm

  // initialisation RTC
  I2C_write(PCF8563,(uint8_t *)time,12);   // tester l'echec

  // EICRA=1<<ISC21; // Int. sur front descendant -- ne PAS activer sinon decl. tt le temps int.
  EIMSK=1<<INT2;  //Autorisation de l'IT INT2

// display initial time
  time[0]=2;                            // registre qu'on veut lire
  I2C_rw(PCF8563, time, 1, time+1, 11);  // lecture de la RTC
  for(i=0; i<11; i++) 
     {if (i<6) time[i+1]=time[i+1]&mask[i];
      buffer[2*i+1]=(time[i+1]&0x0f)+'0';
      buffer[2*i]=((time[i+1]&0xf0)>>4)+'0';
     }
  buffer[22]='\r';buffer[23]='\n';buffer[24]=0; // affichage
  fputs(buffer, &USBSerialStream);

  while (1)
   {time[0]=2;                            // registre qu'on veut lire
    I2C_rw(PCF8563, time, 1, time+1, 9);  // lecture de la RTC
    for(i=0; i<9; i++) 
       {if (i<6) time[i+1]=time[i+1]&mask[i];
        buffer[2*i+1]=(time[i+1]&0x0f)+'0';
        buffer[2*i]=((time[i+1]&0xf0)>>4)+'0';
       }
    buffer[18]='\r';buffer[19]='\n';buffer[20]=0; // affichage
    fputs(buffer, &USBSerialStream);
    _delay_ms(100);                      // attente pour laisser RTC tourner
    if (flag==1) 
      {
       //masquage de l'alarme
       time[0]=1;
       time[1]=0; // 1<<AIE;  // clear AF + reactivate ALM
       I2C_write(PCF8563,(uint8_t *)time,2);

       buffer[0]='*';buffer[1]=0;
       fputs(buffer, &USBSerialStream);
//      CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
//      CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
//      USB_USBTask();

       flag=0;
       PORTD |= 1<<PORTD2; //pullup on <- ne PAS activer
       _delay_ms(10);    
       PORTD &=~1<<PORTD2; //pullup on <- ne PAS activer
      }
    CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    USB_USBTask();

  }
  return 1;
}
