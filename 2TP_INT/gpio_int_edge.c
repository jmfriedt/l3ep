// avr-gcc -mmcu=atmega32u4 -Os  -Wall  -o gpio_int.out gpio_int.c 
// TODO : relier un fil entre GND et D0 (PD2) pour allumer/eteindre la diode orange

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>  // _delay_ms
 
ISR(INT2_vect)
{PORTB^=1<<PORTB5;
 _delay_ms(100);         // software debounce
}	
 
int main(void)
{ MCUCR &=~(1<<PUD);
  DDRD  &= ~(1<<PORTD2); //int2 sur PD2 qui est note' D0
  PORTD |= (1<<PORTD2);  //pullup on, cf doc p62 

  DDRB |=(1<<PORTB5);
  DDRE |=(1<<PORTE6);
  PORTB &= ~(1<<PORTB5);
  PORTE &= ~(1<<PORTE6);

// vv NECESSAIRE SI ON VEUT UTILISER PD0 (sinon le FET d'uext est desactif et GPIO est en pull down)
// DDRB|=1<<PB4;                   // UEXT powered (FET)
// PORTB&=~1<<PB4;
 
  EICRA = 0x22;         // rising edge only
  EIMSK = 1<<INT2;	//enable int2
  USBCON=0;	        // desactive l'interruption sur USB (activ'ee par bootloader)
  sei();		//enable interrupts
  while(1) {PORTE^=1<<PORTE6;_delay_ms(200);} 
}

