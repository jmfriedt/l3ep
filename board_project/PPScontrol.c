// jouer sur f.frequency pour modifier frequence du uC 

#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <pthread.h>
#include <unistd.h> // usleep

#include "sim_avr.h"
#include "avr_ioport.h"
#include "avr_spi.h"
#include "avr_timer.h"
#include "sim_elf.h"
#include "sim_gdb.h"
#include "sim_vcd_file.h"

avr_t * avr = NULL;

volatile int pwm_flag,icp_flag;

volatile uint8_t display_pwm = 0;

void pd2_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param) {printf("PD2\n");}

void icp_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param) { icp_flag=1; }

void pwm_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param) 
     { display_pwm = value; pwm_flag=1; }

static avr_cycle_count_t PPSlo(avr_t * avr, avr_cycle_count_t when, void * param)
{avr_irq_t** irq=(avr_irq_t**) param;
 printf("PPS: lo\n");
 avr_raise_irq(irq[0],1);
 avr_raise_irq(irq[1],1);
}

static avr_cycle_count_t PPSup(avr_t * avr, avr_cycle_count_t when, void * param)
{avr_irq_t** irq=(avr_irq_t**) param;
 //button_press(b, 300000);       // pressed = low
 printf("PPS: up\n");
 avr_raise_irq(irq[0],0);
 avr_raise_irq(irq[1],0);
 // avr_cycle_timer_cancel(avr, PPSup, irq); // inutile cf ../../simavr/sim/sim_cycle_timers.c 
 avr_cycle_timer_register_usec(avr, 1000000, PPSup, irq);                  // qui le fait deja
 // avr_cycle_timer_cancel(avr, PPSlo,NULL);
 avr_cycle_timer_register_usec(avr, 200000, PPSlo, irq);
 return 0;
}

// button_t button;              // ajout du bouton
// // ajout pour la definition du bouton
// button_init(avr, &button, "button"); // initialize our 'peripheral'
// avr_connect_irq(                     // connect the button to the port pin 
//    button.irq + IRQ_BUTTON_OUT,
//    avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 3));

int main(int argc, char *argv[])
{
 elf_firmware_t f;
 avr_irq_t* irq[2];
 pthread_t run;
 const char * fname =  "atmega32u4_PPScontrol.axf";
 elf_read_firmware(fname, &f);
 printf("firmware %s f=%d mmcu=%s\n", fname, (int)f.frequency, f.mmcu);
 avr = avr_make_mcu_by_name(f.mmcu);
 if (!avr) {fprintf(stderr, "%s: AVR '%s' not known\n", argv[0], f.mmcu);exit(1);}
 avr_init(avr);
 avr_load_firmware(avr, &f);

 irq[0]= avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('D'), 2);
 irq[1]= avr_io_getirq(avr, AVR_IOCTL_TIMER_GETIRQ('3'), TIMER_IRQ_IN_ICP);
 //avr_irq_t* icp= avr_get_interrupt_irq(avr, 32); // same effect than TIMER3+TIMER_IRQ_IN_ICP
 avr_irq_t* i_pwm= avr_io_getirq(avr, AVR_IOCTL_TIMER_GETIRQ('0'), TIMER_IRQ_OUT_PWM0);  // detecte changements de PWM
 avr_irq_register_notify(i_pwm, pwm_changed_hook, NULL);  
 avr_irq_register_notify(irq[1], icp_changed_hook, NULL);  
 avr_irq_register_notify(irq[0], pd2_changed_hook, NULL);

// 1-PPS timer init
 avr_cycle_timer_cancel(avr, PPSup, irq);
 avr_cycle_timer_register_usec(avr, 1000000, PPSup, irq);

 pwm_flag=0;
 icp_flag=0;
 while (1)
 { avr_run(avr);
   if (pwm_flag==1) {avr->frequency=16000000+display_pwm*10;
                     printf("PWM:%d -- freq: %d\n",display_pwm,avr->frequency);pwm_flag=0;}
   if (icp_flag==1) {printf("ICP\n");icp_flag=0;}
 }
 return NULL;
}

