#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#define f_CPU 16000000UL
#include <util/delay.h>
#include "libttycom.h"
#include "libi2c.h"

#include <string.h>
#include <stdio.h>

#define USART_BAUDRATE 9600

volatile int alarme=0;

// https://github.com/tomvdb/avr_arduino_leonardo/blob/master/examples/uart/main.c
void uart_transmit(char data )
{while (!(UCSR1A&(1<<UDRE1))) ;
 UDR1 = data;
}

void uart_puts(char *c)
{int k=0;
 while (c[k]!=0) {uart_transmit(c[k]);k++;}
}

void uart_init()
{unsigned short baud;
 UCSR1A = 0;                         // importantly U2X1 = 0
 UCSR1B = 0;
 UCSR1B = (1 << RXEN1)|(1 << TXEN1); // enable receiver and transmitter
 UCSR1C = _BV(UCSZ11) | _BV(UCSZ10); // 8N1
 // UCSR1D = 0;                         // no rtc/cts (probleme de version de libavr)
 baud  = (((( F_CPU / ( USART_BAUDRATE * 16UL))) - 1));
 UBRR1H = (unsigned char)(baud>>8);
 UBRR1L = (unsigned char)baud;
}

void enterSleep(void)
{set_sleep_mode(SLEEP_MODE_IDLE);
 sleep_enable();
 sleep_mode();    // enter sleep mode ... good night
 sleep_disable(); // wake up from interrupt
}

ISR(INT2_vect) //front descendant
{alarme=1;
 PORTB^=1<<PORTB5;
 _delay_ms(100);
 }

int main (void) 
{int8_t dummy;
 init_olimexIno();
 wdt_disable();
 uart_init();
  
 MCUCR &=~1<<PUD;
 DDRD&=~1<<PORTD2; 
 PORTD|=1<<PORTD2; 
  
 DDRB|=1<<PORTB5; //led
 PORTB&=~1<<PORTB5;
  
 DDRE|=1<<PORTE6; //led
 PORTE|=1<<PORTE6;
  
 uint8_t i;                                              //            m_alm          week_alm
 uint8_t mask[13]={0xC8,0x1F,0x7F, 0x7F, 0x3F, 0x3F, 0x07, 0x1F, 0xFF, 0xFF,0xBF,0xBF,0x87}; //correspond aux bits du tableau
 uint8_t time[16]; //le PCF8563 possede 16 registres
 
 char buffer[40]="C'est parti\r\n\0";

 I2C_UEXT(ON); //alimentation du connecteur UEXT on (D8=PB4)
 I2C_init(24,0); //initialisation du bus I2C : 24 pour 400kHz
 I2C_stop();
 
 uart_puts(buffer);
 
 time[0]=1; //adresse du premier registre (ici seconde)
 time[1]=0x02;
 time[2]=0x55; time[3]=01; time[4]=05; time[5]=06; time[6]=7; time[7]=8; time[8]=9; //valeur des registres a partir duquel on veut imposer la valeur
 ////second min hour day weekday month year
  
 //initialisation RTC
 dummy=I2C_write(PCF8563, (uint8_t *) time, 9); //tester l'echec 
 // on donne l'adresse de l'esclave (ici PCF 8563), puis l'adresse du premier registre (time[0]) puis valeur des 8 premiers registres (time[1] à time[7])
 buffer[1]=(dummy&0x0f)+'0';
 buffer[0]=((dummy&0xf0)>>4)+'0';
 //I2C_read(PCF8563, time+1, 7);
 buffer[3]='\r';buffer[4]='\n';buffer[5]=0;
 uart_puts(buffer);

 // initialisation alarme
 time[0]=9; //adresse du premier registre (ici seconde)
 time[1]=0x02; time[2]=0x82; time[3]=0x83; time[4]=0x84;
 
 dummy=I2C_write(PCF8563, (uint8_t *) time, 5); //tester l'echec 
 // on donne l'adresse de l'esclave (ici PCF 8563), puis l'adresse du premier registre (time[0]) puis valeur des 8 premiers registres (time[1] à time[7])
 buffer[1]=(dummy&0x0f)+'0';
 buffer[0]=((dummy&0xf0)>>4)+'0';
 //I2C_read(PCF8563, time+1, 7);
 buffer[3]='\r';buffer[4]='\n';buffer[5]=0;
 uart_puts(buffer);

 EIMSK = 1<<INT2; //enable int2
 EICRA=1<<ISC21;  //front descendant
 sei();

 while(1) 
 {time[0]=1; 
  I2C_rw(PCF8563, time, 1, time+1, 12); //lecteur de la RTC //8
  
  for(i=1; i<=12; i++) 
   {time[i]=time[i]&mask[i]; 
    buffer[2*i-1]=(time[i]&0x0f)+'0';     if (buffer[2*i-1]>'9') buffer[2*i-1]=buffer[2*i-1]-'9'+'A';
    buffer[2*i-2]=((time[i]&0xf0)>>4)+'0';if (buffer[2*i-2]>'9') buffer[2*i-2]=buffer[2*i-2]-'9'+'A';
   }
  buffer[2*12]=0;
  uart_puts(buffer);
   
  if (alarme==1) 
     {sprintf(buffer," ALARME");uart_puts(buffer); 
      time[0]=9;time[1]=time[3]+1; // status=1 seconde=2 minute=3
      I2C_write(PCF8563, (uint8_t *) time, 2);
      time[0]=1;time[1]=0x02;
      I2C_write(PCF8563, (uint8_t *) time, 2);
      alarme=0;
      PORTE ^=1<<PORTE6; //toggle led
      enterSleep(); 
     }
  uart_puts("\r\n\0");
  _delay_ms(1000); //attente pour laisser RTC tourner
 }
 return 0;
} 
  
  
  
