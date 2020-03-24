#include <stdlib.h>
#include <stdio.h>
#include "sim_elf.h"
#include "avr_ioport.h"
#include "button.h"

static avr_cycle_count_t PPS(avr_t * avr, avr_cycle_count_t when, void * param)
{button_t * b = (button_t *)param;
 button_press(b, 300000);       // pressed = low for 300 ms
 printf("PCB: pushed\n");
 avr_cycle_timer_cancel(b->avr, PPS, b);
 avr_cycle_timer_register_usec(b->avr, 1000000, PPS, b); // trigger again in 1s
 return 0;
}

int main(int argc, char *argv[])
{avr_t * avr = NULL;
 elf_firmware_t f;
 button_t button;              // ajout du bouton
 const char * fname =  "atmega32u4_intro.axf";
 elf_read_firmware(fname, &f);
 printf("firmware %s f=%d mmcu=%s\n", fname, (int)f.frequency, f.mmcu);
 avr = avr_make_mcu_by_name(f.mmcu);
 if (!avr) {fprintf(stderr, "%s: AVR '%s' not known\n", argv[0], f.mmcu);exit(1);}
 avr_init(avr);
 avr_load_firmware(avr, &f);
 // add button definition
 button_init(avr, &button, "button"); // initialize our 'peripheral'
 avr_connect_irq(                     // connect the button to the port pin 
    button.irq + IRQ_BUTTON_OUT,
    avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 3));
 avr_raise_irq(button.irq + IRQ_BUTTON_OUT, 1); // raise = pull up
 // 1-PPS timer init: use simavr timer for accurate timing
 avr_cycle_timer_cancel(button.avr, PPS, &button);
 avr_cycle_timer_register_usec(button.avr, 1000000, PPS, &button);
 while (1) {avr_run(avr);
  }
}
