#include <avr/io.h> //E/S ex PORTB 
#define F_CPU 16000000UL
#include <util/delay.h>  // _delay_ms

#include "avr_mcu_section.h"
AVR_MCU(F_CPU, "atmega32");

const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
		{ AVR_MCU_VCD_SYMBOL("PORTB"), .what = (void*)&PORTB, },
	};

int main(void){
  DDRB |=1<<PORTB5;
  PORTB |= 1<<PORTB5;

  while (1){
    /* Clignotement des LEDS */
    PORTB^=1<<PORTB5;
    _delay_ms(1000);  //Attente de 500ms
  }
  return 0;
}
