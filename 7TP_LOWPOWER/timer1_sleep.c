#include <avr/io.h> //E/S ex PORTB 
#include"libttycom.h"
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>  // _delay_ms
#include "USBAPI.h"       // USB_writestr()

volatile int f_timer=0;

ISR(TIMER1_OVF_vect) {if(f_timer == 0) f_timer = 1;}

void enterSleep(void)
{
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  sleep_mode(); // enter sleep mode ... good night
  sleep_disable(); // wake up from interrupt
}

void setup()
{
  DDRB |=1<<PORTB5;    // LED
  PORTB |= 1<<PORTB5;
  
  TCCR1A = 0x00; // normal timer (overflow)
  TCNT1=0x0000; // clear timer counter 
  TCCR1B = 0x05; // prescaler: 1 s
  TIMSK1=0x01; // timer overlow interrupt
}

int main()
{init_olimexIno();
 setup();
 sei();

 while (1)
 {

  if(f_timer==1)
  {f_timer = 0;
   PORTB ^= 1<<PORTB5; // toggle LED 
   enterSleep();       // 34 mA si inactif, 26 mA si actif
  }
 }
}
