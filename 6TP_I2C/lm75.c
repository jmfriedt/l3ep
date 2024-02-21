#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "libttycom.h"
#include "libi2c.h"        // definit PCF8563=0xA2

#define LM75 0x90

void affiche(unsigned char *b,unsigned char *t)
{ [... fournir la fonction qui affiche la temperature lue sur le LM75 ...]
}

int main(void){
  uint8_t temperature[3];   
  uint8_t buffer[19]="Bonjour";
  init_olimexIno();               //Initialisation de la carte
  sei();
  USB_init();                     // attente de connexion avec minicom
  sprintf((char *) buffer,"Bonjour tout le monde!\n\r");
  USB_writestr((char *) buffer);
  I2C_UEXT(ON);                   // Alimentation du connecteur UEXT on
  I2C_init(24, 0);                // Initialisation du bus I2C : 24 pour 400kHz
  I2C_stop();
  
  temperature[0]=0;
  buffer[2]=0;
  while (1)
   {_delay_ms(100);
    // I2C_rw(LM75, temperature, 1, temperature+1, 2);
    I2C_write(LM75, temperature, 1);
    I2C_read(LM75|1, temperature+1, 2);
    affiche(buffer,&temperature[1]);
    sprintf((char*)&buffer[4]," %03d.%01d\r\n",temperature[1],(temperature[2]>>7)*5); // 9 bits
// *(short*)temperature: intervertit les octets car on n'est pas sur la bonne endianness !
    USB_writestr((char *)buffer); 
   }
  return 1;
}
