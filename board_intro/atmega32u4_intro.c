#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>  // _delay_ms

// simulator variables: CPU type (not used in the firmware)
#include "avr_mcu_section.h"     
AVR_MCU(F_CPU, "atmega32u4"); 

int main(void)
{int masque=(1<<PORTB5);
 DDRB =0xff;
 PORTB=0;
 while(1)
  {PORTB ^= masque;
   if ((PINC&(1<<3))==0)  // pressed: button goes low
     {PORTB=0;
      if (masque<0x80) masque=masque<<1;
         else masque=0x01;
     }
   _delay_ms(100);
  }
}
