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

#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <IL/il.h>
#include <GL/glut.h>
#endif

#define with_GUI

volatile int pd2_flag,pwm_flag,icp1_flag,icp3_flag,PPS_flag; // global variables used in callbacks

// GUI handling
#ifdef with_GUI
int *imageGL;

void my_timer(int x) {glutTimerFunc(40, my_timer, 1);glutPostRedisplay();}

void display(void)
{float size=20.,sizeicp=10.;
 glClear ( GL_COLOR_BUFFER_BIT );
 glRasterPos2i(0,0);
 glDrawPixels(800,600,GL_RGB, GL_UNSIGNED_INT, imageGL);
 glPointSize(10.0);
 if (PPS_flag)
   {glColor3f ( 1.0f, 0.0f, 0.0f );
    glBegin(GL_POLYGON);
    glVertex2f(115.-size, 425.-size);
    glVertex2f(115.-size, 425.+size);
    glVertex2f(115.+size, 425.+size);
    glVertex2f(115.+size, 425.-size);
    glEnd();
   }
 if (icp3_flag)
   {glColor3f ( 1.0f, 1.0f, 0.0f );
    glBegin(GL_POLYGON);
    glVertex2f(400.-sizeicp, 400.-sizeicp);
    glVertex2f(400.-sizeicp, 400.+sizeicp);
    glVertex2f(400.+sizeicp, 400.+sizeicp);
    glVertex2f(400.+sizeicp, 400.-sizeicp);
    icp3_flag=0;
    glEnd();
   }
 if (icp1_flag)
   {glColor3f ( 0.0f, 1.0f, 1.0f );
    glBegin(GL_POLYGON);
    glVertex2f(440.-sizeicp, 400.-sizeicp);
    glVertex2f(440.-sizeicp, 400.+sizeicp);
    glVertex2f(440.+sizeicp, 400.+sizeicp);
    glVertex2f(440.+sizeicp, 400.-sizeicp);
    icp1_flag=0;
    glEnd();
   }
 glutSwapBuffers();
}

void myreshape(int h, int w)
{glMatrixMode (GL_PROJECTION);
 glLoadIdentity();
 gluOrtho2D(0.0, (GLfloat) 800, 0.0, (GLfloat) 600);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 glViewport(0,0,h,w);
}

////////////// load JPEG image: all code stolen from ... ////////////////
// https://community.khronos.org/t/how-to-load-an-image-in-opengl/71231/6
// libdevil-dev - Cross-platform image loading and manipulation toolkit
 
#define DEFAULT_WIDTH  800
#define DEFAULT_HEIGHT 600
 
int width  = DEFAULT_WIDTH;
int height = DEFAULT_HEIGHT;

/* Load an image using DevIL and return the devIL handle (-1 if failure) */
int LoadImage(char *filename)
{ILuint    image; 
 ILboolean success; 
 ilGenImages(1, &image);    // Generation of one image name 
 ilBindImage(image);        // Binding of image name 
 if ( success = ilLoadImage(filename) ) // Loading image by DevIL 
    { // Convert every colour component into unsigned byte 
     success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); 
     if (!success) return -1;
    }
    else return -1;
    return image;
}
#endif
///////////////////////////////////

avr_t * avr = NULL;

volatile uint8_t display_pwm = 0;

void pd2_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param)  {pd2_flag=1; }
void icp1_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param) {icp1_flag=1;}
void icp3_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param) {icp3_flag=1;}
void pwm_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param) 
     { display_pwm = value; pwm_flag=1; }

static avr_cycle_count_t PPSlo(avr_t * avr, avr_cycle_count_t when, void * param)
{avr_irq_t** irq=(avr_irq_t**) param;
#ifndef with_GUI
 printf("PPS: lo\n");
#endif
 PPS_flag=0;
 avr_raise_irq(irq[0],1); // PD2
 avr_raise_irq(irq[1],1); // ICP3
 avr_raise_irq(irq[2],1); // ICP1
 return 0;
}

static avr_cycle_count_t PPSup(avr_t * avr, avr_cycle_count_t when, void * param)
{avr_irq_t** irq=(avr_irq_t**) param;
 //button_press(b, 300000);       // pressed = low
#ifndef with_GUI
 printf("PPS: up\n");
#endif
 PPS_flag=1;
 avr_raise_irq(irq[0],0); // PD2
 avr_raise_irq(irq[1],0); // ICP3
 avr_raise_irq(irq[2],0); // ICP1
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

static void * avr_run_thread(void * param)
{avr_irq_t** irq=(avr_irq_t**) param;
 pwm_flag=0;
 icp1_flag=0;
 icp3_flag=0;
// 1-PPS timer init
 avr_cycle_timer_cancel(avr, PPSup, irq);
 avr_cycle_timer_register_usec(avr, 1000000, PPSup, irq);

 while (1)
 { avr_run(avr);
   if (pwm_flag==1) {avr->frequency=16000000+display_pwm*10;
                     printf("PWM:%d freq: %d\n",display_pwm,avr->frequency);pwm_flag=0;}
#ifndef with_GUI
   if (icp1_flag==1) {printf("ICP1\n");icp1_flag=0;}
   if (icp3_flag==1) {printf("ICP3\n");icp3_flag=0;}
   if (pd2_flag==1) {printf("PD2\n");pd2_flag=0;}
#endif
 }
 return NULL;
}

int main(int argc, char *argv[])
{int x,y, image; // JPEG image
 elf_firmware_t f;
 avr_irq_t* irq[3];
 pthread_t run;
 const char * fname =  "atmega32u4_PPScontrol.axf";
 elf_read_firmware(fname, &f);
 printf("firmware %s f=%d mmcu=%s\n", fname, (int)f.frequency, f.mmcu);
 avr = avr_make_mcu_by_name(f.mmcu);
 if (!avr) {fprintf(stderr, "%s: AVR '%s' not known\n", argv[0], f.mmcu);exit(1);}
 avr_init(avr);
 avr_load_firmware(avr, &f);

 irq[0]= avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('D'), 2);
 irq[1]= avr_io_getirq(avr, AVR_IOCTL_TIMER_GETIRQ('3'), TIMER_IRQ_IN_ICP); // ICP3
 irq[2]= avr_io_getirq(avr, AVR_IOCTL_TIMER_GETIRQ('1'), TIMER_IRQ_IN_ICP); // ICP1
// avr_irq_t* icp= avr_get_interrupt_irq(avr, 32); // same effect than TIMER3+TIMER_IRQ_IN_ICP
 avr_irq_t* i_pwm= avr_io_getirq(avr, AVR_IOCTL_TIMER_GETIRQ('0'), TIMER_IRQ_OUT_PWM0);  // detecte changements de PWM
 avr_irq_register_notify(i_pwm, pwm_changed_hook, NULL);  
 avr_irq_register_notify(irq[1], icp3_changed_hook, NULL);  
 avr_irq_register_notify(irq[2], icp1_changed_hook, NULL);  
 avr_irq_register_notify(irq[0], pd2_changed_hook, NULL);

 pthread_create(&run, NULL, avr_run_thread, irq);

#ifdef with_GUI
 if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION) // Init DevIL 
     { printf("wrong DevIL version "); return -1; }
 ilInit(); 
 image = LoadImage("picture.jpg"); // load the file picture with DevIL 
 if ( image == -1 ) { printf("Can't load picture file"); return -1; }
 ILubyte * bytes = ilGetData();
 ILuint width,height;
 width = ilGetInteger(IL_IMAGE_WIDTH);
 height = ilGetInteger(IL_IMAGE_HEIGHT);
 imageGL = (int*)malloc(3*sizeof(GLint)*width*height);
 for(x = 0; x < width; x++)
  for(y = height-1; y >= 0; y--)  // n=height
  {imageGL[ 3*(y*width + x)+1 ] = bytes[(y*width + x)*4 + 1];
   imageGL[ 3*(y*width + x)+2 ] = bytes[(y*width + x)*4 + 2];
   imageGL[ 3*(y*width + x)+0 ] = bytes[(y*width + x)*4 + 0]; 
  }
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 glutInitWindowSize(width,height);
 glutInitWindowPosition(0,0);
 glutCreateWindow(argv[0]);       // exec name
 glutReshapeFunc(myreshape);
 glutDisplayFunc(display);
 glPixelTransferf(GL_RED_SCALE, 1.);
 glPixelTransferf(GL_GREEN_SCALE, 1.);
 glPixelTransferf(GL_BLUE_SCALE, 1.);
 glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE);
 glClearColor(1.0, 1.0, 1.0, 1.0);
 glutTimerFunc(40, my_timer,1);
 glutMainLoop();
#else
 while (1) {} // don't leave me
#endif

 //ilDeleteImages(1, &image);  // we never get there (JMF) :(
}

