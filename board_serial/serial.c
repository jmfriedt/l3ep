// Makefile : add ${board} : ${OBJ}/uart_pty.o and -lutil
// install picocom and export SIMAVR_UART_XTERM=1

#include <stdlib.h>
#include <stdio.h>

#include "sim_avr.h"
#include "avr_ioport.h"
#include "sim_elf.h"
#include "sim_gdb.h"
#include "sim_vcd_file.h"

#include "uart_pty.h"
uart_pty_t uart_pty;

avr_t * avr = NULL;
avr_vcd_t vcd_file;
int window;
elf_firmware_t f;

int main(int argc, char *argv[])
{const char * fname =  "atmega32u4_serial.axf";
 elf_read_firmware(fname, &f);
 printf("firmware %s f=%d mmcu=%s\n", fname, (int)f.frequency, f.mmcu);
 avr = avr_make_mcu_by_name(f.mmcu);
 if (!avr) {fprintf(stderr, "%s: AVR '%s' not known\n", argv[0], f.mmcu);exit(1);}
 avr_init(avr);
 avr_load_firmware(avr, &f);

 uart_pty_init(avr, &uart_pty);
 uart_pty_connect(&uart_pty, '1');

 while (1) {avr_run(avr);}
}
