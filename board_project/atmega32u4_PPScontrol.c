// input capture (ICP3=PC7, ICP1=PD4)
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>  // _delay_ms

// simulator variables: trace file + CPU type (not used in the firmware)
#include "avr_mcu_section.h"     
AVR_MCU(F_CPU, "atmega32u4"); 
AVR_MCU_VCD_FILE("trace_file.vcd", 1000);
const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
        { AVR_MCU_VCD_SYMBOL("PORTB"), .what = (void*)&PORTB, },
        { AVR_MCU_VCD_SYMBOL("UDR1"), .what = (void*)&UDR1, },
};
AVR_MCU_VCD_ALL_IRQ();  // also show ALL irqs running

// global variables for sharing between ISR and main
volatile int flag_int2,flag_icp,value_icp;

ISR(INT2_vect)
{
 flag_int2=1;
}	
 
#define USART_BAUDRATE 9600

void transmit_data(uint8_t data)
{while ( !( UCSR1A & (1<<UDRE1)) );
 UDR1 = data;
}

void usart_setup()              // initialisation UART
{unsigned short baud;
 baud  = (((( F_CPU / ( USART_BAUDRATE * 16UL))) - 1));
 DDRD  |= 0x18;
 UBRR1H = (unsigned char)(baud>>8);
 UBRR1L = (unsigned char)baud;
		
 UCSR1C = (1<<UCSZ11) | (1<<UCSZ10); //Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
 UCSR1B = (1<<RXEN1 ) | (1<<TXEN1 ); //Enable Transmitter and Receiver
}

int main(void)
{char symbole='.';
 MCUCR &=~(1<<PUD);
 DDRB |=1<<PORTB5;      // blinking LED
 PORTB |= 1<<PORTB5;

 DDRD  &= ~(1<<PORTD2); // int2 sur PD2 qui est note' D0
 PORTD |= (1<<PORTD2);  // pullup on, cf doc p62 
 EICRA=0xff;            // rising edge on all pin interrupts
 EIMSK = 1<<INT2;	// enable int2 sur PD2
  
 usart_setup();
 
 sei();

 flag_int2=0;	
 while(1)
  {transmit_data(symbole);
   if (flag_int2!=0) {transmit_data('2'); flag_int2=0;}
   PORTB^=(1<<PORTB5); // bouge PORTB pour generer le fichier vcd a regarder avec gtkwave
   _delay_ms(200);
  }
}
