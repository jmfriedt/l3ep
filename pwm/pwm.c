#include <avr/io.h>

void pwm_configure()        // called once for initialization
{TCCR4A=0;
 TCCR4B=(1<<CS40);
 TCCR4C=0;
 TCCR4D=0;
 TC4H=0x03;
 OCR4C=255;
 TCCR4C|=0x09;              // Activate channel D
 DDRD|=1<<7;                // Set GPIO Output Mode
}

void pwmD_configure(short val) // Timer4 D: update duty cycle to val
{TC4H=(val>>8)&3;
 OCR4D=(char)(val&0xff);
}
