#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/wdt.h>     // voir /usr/lib/avr/include//avr/wdt.h pour les cst
#include <util/delay.h>  // _delay_ms
#include <avr/power.h>
#include <avr/interrupt.h>

#include "VirtualSerial.h"

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;
extern FILE USBSerialStream;

int main (void)
{DDRB |=1<<PORTB5;
 PORTB |= 1<<PORTB5;

 _delay_ms(1000); 
 SetupHardware();
 CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
 GlobalInterruptEnable();  // sei();	

 wdt_enable(WDTO_120MS);
 while (1)
   {wdt_reset(); // pour reinitialiser le WDT
    PORTB&=~1<<PORTB5;
    _delay_ms(20); // 20 ms ici + USB ~ 120 ms
    fprintf(&USBSerialStream,".");

    CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    USB_USBTask();
   }
}
