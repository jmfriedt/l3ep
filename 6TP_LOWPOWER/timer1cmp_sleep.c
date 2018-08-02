#include <avr/io.h> //E/S ex PORTB 
#include"libttycom.h"
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>  // _delay_ms
#include "USBAPI.h"       // USB_writestr()

volatile int res=0,tim1=0,gpio0=0;
volatile unsigned int delai_leds=(16000*4);

ISR (TIMER1_COMPA_vect) 
  {PORTB^=1<<PORTB5;PORTE^=1<<PORTE6;
   tim1='A';
  }

ISR(INT0_vect)
{gpio0='B';
 _delay_ms(100); // software debounce
}	

void enterSleep(void)
{
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  sleep_mode(); // enter sleep mode ... good night
  sleep_disable(); // wake up from interrupt
}

void setup()
{
// timer1 in output compare mode
  TCCR1B |= (1 << WGM12)|(1 << CS11)|(1 << CS10);
  OCR1A = delai_leds;      // valeur seuil <- delai
  TIMSK1 |= (1 << OCIE1A);
  
  MCUCR &=~1<<PUD;    //pull up enable
  DDRD  |= 1<<PORTD0; //int0 sur PD0 qui est note' D3
  PORTD |= 1<<PORTD0; //pullup on 
  EIMSK = 1<<INT0;    //enable int0 : relier un fil entre GND et D3
}

int main()
{char c[10]="     \r\n\0";
 init_olimexIno();
 setup();
 sei();
 USB_init();   // apres sei()
 enterSleep();

 while (1)
 {if (tim1 !=0) {c[0]=tim1; USB_writestr(c); tim1=0;enterSleep();}
  if (gpio0!=0) {c[0]=gpio0;USB_writestr(c);gpio0=0;enterSleep();}
 }
}


