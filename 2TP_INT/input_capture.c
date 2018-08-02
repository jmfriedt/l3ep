#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>  // _delay_ms

#undef rs  // affichage sur RS232 (#define) ou sur USB (#undef)

volatile unsigned short res;

#ifdef rs
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
#else

#include "VirtualSerial.h"

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;
extern FILE USBSerialStream;

void transmit_data(uint8_t data)
{char buffer[2];
 buffer[0]=data;buffer[1]=0;
 fputs(buffer, &USBSerialStream);

}
#endif

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

ISR(TIMER3_CAPT_vect)
{res  = ICR3;
 TCNT3 = 0;
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
{
  DDRB |=1<<PORTB5;
  DDRE |=1<<PORTE6;
  PORTB |= 1<<PORTB5;
  PORTE &= ~1<<PORTE6;
  mypwm_setup();
  myicp_setup();
//  sei();
#ifdef rs
  usart_setup();
#else
  USBCON=0;
  SetupHardware();
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
  GlobalInterruptEnable();
#endif
	
 while(1)
  {if (res!=0) {write_short(res); transmit_data(0x0A); transmit_data(0x0D); res=0;
  }
#ifdef rs
#else
     CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);

     CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
     USB_USBTask();
#endif
   //_delay_ms(500);
  }
}
