// simavr detecte l'interruption sur PD2 mais pas le programme 32U4
// pas d'evenement sur input capture (ICP3=PC7, ICP1=PD4)

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>  // _delay_ms

// for linker, emulator, and programmer's sake
#include "avr_mcu_section.h"
//AVR_MCU(F_CPU, "atmega32");
AVR_MCU(F_CPU, "atmega32u4");  // needed to identify core
AVR_MCU_VCD_FILE("trace_file.vcd", 1000);
const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
        { AVR_MCU_VCD_SYMBOL("PORTB"), .what = (void*)&PORTB, },
        { AVR_MCU_VCD_SYMBOL("UDR1"), .what = (void*)&UDR1, },
};
AVR_MCU_VCD_ALL_IRQ();  // also show ALL irqs running

volatile unsigned short res;
volatile int flag;

// avr-gcc -mmcu=atmega32u4 -Os  -Wall  -o gpio_int.out gpio_int.c 
// TODO : relier un fil entre GND et D0 (PD2) pour allumer/eteindre la diode orange
 
ISR(INT2_vect)
{
 flag=1;
}	
 
ISR(TIMER3_CAPT_vect)
{res  = ICR3;
 TCNT3 = 0;
// TIFR3  |= 1<<ICF3; cleared automatically
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

void write_char(unsigned char c)
{if ((c>>4)>9) transmit_data((c>>4)-10+'A'); else transmit_data((c>>4)+'0');
 if ((c&0x0f)>9) transmit_data((c&0x0f)-10+'A'); else transmit_data((c&0x0f)+'0');
}

void write_short(unsigned short s)
{write_char(s>>8);
 write_char(s&0xff);
}

void myicp_setup() // initialisation timer3
{TCCR3A = 1<<WGM31 | 1<<WGM30;
 TCCR3B = 1<<ICES3 | 1<<WGM33 | 1<<CS31;
 OCR3A  = 32000;
 TIMSK3 = 1<<ICIE3;
 TIFR3  = 1<<ICF3;
}

#define PWM_MAX_DUTY_CYCLE 0xFF
uint8_t pwmRunning = PWM_MAX_DUTY_CYCLE - (PWM_MAX_DUTY_CYCLE >> 4);
void pwm0Init(void)
{
    /* 
       Start Timer 0 with no clock prescaler and phase correct 
       fast PWM mode. Output on PD6 (OC0A). 
    */
    TCCR0A =  (1<<COM0A1)|(0<<COM0A0)|(0<<COM0B1)|(0<<COM0B0)
             |(1<<WGM01) |(1<<WGM00);
    TCCR0B =  (0<<FOC0A) |(0<<FOC0B) |(0<<WGM02)
             |(0<<CS01)  |(1<<CS00);
//      TIMSK0 = (1 << OCIE0A);
        
    // Reset counter
    TCNT0 = 0;

    // Set duty cycle to 1/16th duty
//    OCR0A  = PWM_MAX_DUTY_CYCLE - (PWM_MAX_DUTY_CYCLE >> 4);
        OCR0A = pwmRunning;
}


// http://blog.saikoled.com/post/43165849837/secret-konami-cheat-code-to-high-resolution-pwm-on-your
void mypwm_setup()  {
  DDRB |= (1<<7)|(1<<6)|(1<<5); // B5/OC1A, B6/OC1B, B7/OC1C 

  // TCCR1A is the Timer/Counter 1 Control Register A.
  // TCCR1A[7:6] = COM1A[1:0] (Mode for Channel A)
  // TCCR1A[5:4] = COM1B[1:0] (Mode for Channel B)
  // TCCR1A[3:2] = COM1C[1:0] (Mode for Channel C)
  // COM1x[1:0] = 0b10 (Clear on match. Set at TOP)

  // Clock Prescaler Stuff
  // TCCR1B[2:0] = CS1[2:0] (Clock Select for Timer 1)
  // CS1[2:0] = 0b001 (No Prescaler)

  // Waveform Generation Mode Setup Stuff

  // TCCR1A[1:0] = WGM1[1:0] (WGM for Timer 1 LSB)
  // TCCR1B[4:3] = WGM1[3:2] (WGM for Timer 1 MSB)
  // WGM[3:0] = 0b1110 (i.e. Mode 14)
  // Mode 14 - Fast PWM, TOP from ICR1, Set OC1x at
  // TOP, clear at OCR1x.

  // ICR1 is a 16-bit register that should be written
  // with the desired value for TOP. This is the value
  // at which the PWM cycle restarts and defines the
  // resolution.

  // For 16-bit resolution, we need to program ICR1 with
  // 0xFFFF.
  ICR1 = 0x3e80;  // intervalle de repetition du crenau = 1 ms
  TCCR1A = 0xAA;
  TCCR1B = 0x19;

  OCR1A=0x3e80/2; // largeur du crenau = 1/2 ms
}

int main(void)
{ char symbole='.';
  transmit_data('h');
  MCUCR &=~(1<<PUD);
  DDRB |=1<<PORTB5;
  DDRE |=1<<PORTE6;
  PORTB |= 1<<PORTB5;
  PORTE &= ~1<<PORTE6;

  DDRD  &= ~(1<<PORTD2); // int2 sur PD2 qui est note' D0
  PORTD |= (1<<PORTD2);  // pullup on, cf doc p62 
  mypwm_setup();
  myicp_setup();
  pwm0Init();     // test JMF
  sei();
  usart_setup();

  EIMSK = 1<<INT2;	// enable int2 sur PD2

 flag=0;	
 while(1)
  {transmit_data(symbole);
   if (res!=0) {symbole='V'; res=0;}
   if (flag!=0) {symbole='u'; flag=0;}
   OCR0A++;       // modifie le rapport cylique de la PWM -> declenche un evenement sur le PCB
   _delay_ms(100);
  }
}

