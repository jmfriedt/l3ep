#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#include "VirtualSerial.h"
#include <util/delay.h>

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;
extern FILE USBSerialStream;

int main(void)
{
 char ReportString[20] = "World\r\n\0";

 SetupHardware();
/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
  GlobalInterruptEnable();

  for (;;)
    {
     fprintf(&USBSerialStream,"Hello ");
     fputs(ReportString, &USBSerialStream);

//   les 3 lignes ci-dessous pour accepter les signaux venant du PC 
     CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);

     CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
     USB_USBTask();
     _delay_ms(500);
    }
}
