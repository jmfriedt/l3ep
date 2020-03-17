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

volatile int pwm_flag,alarm_flag,icp_flag;

avr_t * avr = NULL;
avr_vcd_t vcd_file;

volatile uint8_t display_pwm = 0;

void handle_alarm(int xxx) { alarm_flag=1; alarm(1); } // 1 PPS cote' PC (board)
void init_alarm(void) { signal(SIGALRM, handle_alarm); alarm(1);}  // signal 1 PPS

void pd2_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param) {printf("PD2\n");}

void icp_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param) { icp_flag=1; }

void pwm_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param) 
     { display_pwm = value; pwm_flag=1; }

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

 avr_irq_t* pd2= avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('D'), 2);
 avr_irq_t* icp= avr_io_getirq(avr, AVR_IOCTL_TIMER_GETIRQ('3'), TIMER_IRQ_IN_ICP);
 //avr_irq_t* icp= avr_get_interrupt_irq(avr, 32); // same effect than TIMER3+TIMER_IRQ_IN_ICP
 avr_irq_t* i_pwm= avr_io_getirq(avr, AVR_IOCTL_TIMER_GETIRQ('0'), TIMER_IRQ_OUT_PWM0);  // detecte changements de PWM
 avr_irq_register_notify(i_pwm, pwm_changed_hook, NULL);  
 avr_irq_register_notify(icp, icp_changed_hook, NULL);  
 avr_irq_register_notify(pd2, pd2_changed_hook, NULL);

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
        if (pwm_flag==1) {f.frequency=16000000+display_pwm*10;
                          printf("PWM:%d -- freq: %d\n",display_pwm,f.frequency);pwm_flag=0;}
        if (icp_flag==1) {printf("ICP\n");icp_flag=0;}
        if (alarm_flag==1) 
                   {printf("PPS:");alarm_flag=0;
                    avr_raise_irq(pd2,0);
                    avr_raise_irq(icp,0);
                    usleep(100);
                    avr_raise_irq(icp,1);
                    avr_raise_irq(pd2,1);
                   }
            }
}