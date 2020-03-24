#include <stdlib.h>
#include <stdio.h>
#include "sim_elf.h"

int main(int argc, char *argv[])
{avr_t * avr = NULL;
 elf_firmware_t f;
 const char * fname =  "a.out";// "atmega32u4_intro.axf";
 elf_read_firmware(fname, &f);
 printf("firmware %s f=%d mmcu=%s\n", fname, (int)f.frequency, f.mmcu);
 avr = avr_make_mcu_by_name(f.mmcu);
 if (!avr) {fprintf(stderr, "%s: AVR '%s' not known\n", argv[0], f.mmcu);exit(1);}
 avr_init(avr);
 avr_load_firmware(avr, &f);
 while (1) {avr_run(avr);}
}
