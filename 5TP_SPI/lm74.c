#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "VirtualSerial.h"

#define cs_lo PORTC &= ~(1 << PORTC7)     // CS#
#define cs_hi PORTC |= (1 << PORTC7)

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;
extern FILE USBSerialStream;

void init_SPI()
{DDRB |= ((1<<DDB0)|(1 << DDB2)| (1 << DDB1)); // MOSI (B2), SCK (B1) en sortie, MISO (B3) en entree
 DDRB &= ~(1 << DDB3);    // ATTENTION : meme si CS manuel, DDB0 doit etre out pour ne pas bloquer SPI
 DDRC |= (1 << DDC7);     // CS#                    // p.17.2.1 datasheet

 SPCR = (0<<SPIE) | (1<<SPE)  | (0<<DORD) | (1<<MSTR) | (0 << CPOL) | (0<<CPHA) | (1<<SPR1) | (1<<SPR0);
//        Int Ena | SPI ENA   | 0=MSB 1st | Master    | CK idle hi  | sample trailing SCK |  f_OSC / 128 
 SPSR &= ~(1 << SPI2X); // No doubled clock frequency
}

char sd_raw_send_byte(char b)
{SPDR = b;                  // emet MOSI
 while(!(SPSR & (1 << SPIF)));
 SPSR &= ~(1 << SPIF);
 return SPDR;               // renvoie MISO
}

int main()
{ char buffer[80]="Let's go\r\n\0";
  unsigned char ch,cl; // DOIVENT etre unsigned (sinon erreur lors de la concatenation)
  int k; int32_t temperature;
  SetupHardware();
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
  GlobalInterruptEnable();

  DDRB|=1<<PB4;                   // UEXT powered (FET)
  PORTB&=~1<<PB4;
  init_SPI();
  
  fputs(buffer, &USBSerialStream);
  while (1) {
    cs_lo;           // active le capteur
    ch=sd_raw_send_byte(0x00);// binaire(ch,buffer);
    cl=sd_raw_send_byte(0x00);// binaire(cl,&buffer[8]);
    for (k=0;k<2;k++) {sd_raw_send_byte(0x00);}
    sprintf(&buffer[16]," %04x",ch*256+cl);
    temperature=(((int32_t)(ch*256+cl)>>3)*625)/10;
    sprintf(&buffer[21]," %03ld.%03ld\r\n",temperature/1000,temperature-(temperature/1000)*1000);
    cs_hi;

//    buffer[16]='\r';buffer[17]='\n';buffer[18]=0;
    fputs(buffer, &USBSerialStream);

    _delay_ms(100);
    CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    USB_USBTask();
  }
  return(0);
}
