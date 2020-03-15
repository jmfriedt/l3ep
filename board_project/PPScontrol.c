// jouer sur f.frequency pour modifier frequence du uC

#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include "sim_avr.h"
#include "avr_ioport.h"
#include "avr_spi.h"
#include "avr_timer.h"
#include "sim_elf.h"
#include "sim_gdb.h"
#include "sim_vcd_file.h"

#include "button.h"

volatile int pwm_flag,alarm_flag,icp_flag;

button_t buttond2,buttonc7; 
avr_t * avr = NULL;
avr_vcd_t vcd_file;

volatile uint8_t display_pwm = 0;

void handle_alarm(int xxx) { alarm_flag=1; alarm(1); } // 1 PPS cote' PC (board)
void init_alarm(void) { signal(SIGALRM, handle_alarm); alarm(1);}  // signal 1 PPS

void pd2_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param) {printf("PD2\n\n");}

void icp_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param) { icp_flag=1; }

void pwm_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param) { display_pwm = value; pwm_flag=1; }

int main(int argc, char *argv[])
{init_alarm();
 elf_firmware_t f;
 const char * fname =  "atmega32u4_PPScontrol.axf";
 elf_read_firmware(fname, &f);
 printf("firmware %s f=%d mmcu=%s\n", fname, (int)f.frequency, f.mmcu);
 avr = avr_make_mcu_by_name(f.mmcu);
 if (!avr) {fprintf(stderr, "%s: AVR '%s' not known\n", argv[0], f.mmcu);exit(1);}
 avr_init(avr);
 avr_load_firmware(avr, &f);

// JMF https://github.com/buserror/simreprap/blob/master/shared/simavr/simavr/sim/avr_timer.c
 button_init(avr, &buttond2, "buttonD2");
 button_init(avr, &buttonc7, "buttonC7");
 avr_connect_irq(buttond2.irq +IRQ_BUTTON_OUT,avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('D'), 2));
 avr_connect_irq(buttonc7.irq +IRQ_BUTTON_OUT,avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 7)); // pour message 'ICP'
	
 //avr_irq_t* icp= avr_io_getirq(avr, AVR_IOCTL_TIMER_GETIRQ('3'), TIMER_IRQ_IN_ICP);
 avr_irq_t* icp= avr_get_interrupt_irq(avr, 31); // same effect than TIMER3+TIMER_IRQ_IN_ICP
 avr_irq_t* i_pwm= avr_io_getirq(avr, AVR_IOCTL_TIMER_GETIRQ('0'), TIMER_IRQ_OUT_PWM0);  // detecte changements de PWM
 avr_irq_register_notify(i_pwm, pwm_changed_hook, NULL);  
 avr_irq_register_notify(icp, icp_changed_hook, NULL);  
 avr_irq_register_notify(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('D'), 2), pd2_changed_hook, NULL);

 avr_vcd_init(avr, "gtkwave_output.vcd", &vcd_file, 10000 /* usec */);
 avr_vcd_add_signal(&vcd_file, avr_get_interrupt_irq(avr, 7), 1 /* bit */ ,
		"TIMER2_COMPA" );

//	avr_vcd_add_signal(&vcd_file, i_pwm, 8 /* bits */ ,
//		"PWM" );

  pwm_flag=0;
  icp_flag=0;
  alarm_flag=0;
  while (1) {
	avr_run(avr);
        if (pwm_flag==1) {printf("PWM:%d\n",display_pwm);pwm_flag=0;}
        if (icp_flag==1) {printf("ICP\n");icp_flag=0;}
        if (alarm_flag==1) 
                   {printf("PCalarm\n");alarm_flag=0;//avr_raise_irq(icp,1); 
                    avr_raise_irq(buttond2.irq+IRQ_BUTTON_OUT,1);
                    avr_raise_irq(buttonc7.irq+IRQ_BUTTON_OUT,1);
                   }
            }
}
