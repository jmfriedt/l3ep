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

#define USART_BAUDRATE 9600

volatile char received,flag;

ISR(USART1_RX_vect) {received=UDR1;flag=1;}

void mytransmit_uart(uint8_t data)
{while ( !( UCSR1A & (1<<UDRE1)) );
 UDR1 = data;
}

unsigned char myreceive_uart(void)   // polling mode
{while (!(UCSR1A&(1<<RXC1))) ;
 return UDR1;
}

void usart_setup()             
{unsigned short baud;
 baud  = (((( F_CPU / ( USART_BAUDRATE * 16UL))) - 1));
 DDRD  |= 0x18;
 UBRR1H = (unsigned char)(baud>>8);
 UBRR1L = (unsigned char)baud;
 UCSR1A = 0;                         // importantly U2X1 = 0
 UCSR1C = (1<<UCSZ11) | (1<<UCSZ10); // asynch, no parity, 1 stop, 8 bit size
 UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1); ; // TX, RX & RX interrupt
}

int main(void)
{char symbole='.';
 DDRB=0xff;
 usart_setup();
 flag=0;
 sei();
 while(1)
  {mytransmit_uart(symbole);
//   symbole=myreceive_uart();  // polling mode
   if (flag!=0) {flag=0;symbole=received+1;}
//   PORTB^=0x10;
   _delay_ms(200);
  }
}
