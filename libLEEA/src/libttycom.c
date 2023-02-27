/*
 * Bibliothèque de pilotage de  l'afficheur EzDisplay
 * Auteurr M.Bernier
 * Date : 12/10/2008
*/
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include"libttycom.h"
#include "USBAPI.h"


void init_olimexIno(){
  //Déactivation les interruptions du bootloader !!!
  TIMSK0=0;TIMSK1=0;  //TIMSK2=0;  
  TIMSK3=0;TIMSK4=0;  //TIMSK5=0;
  //initiatisation pour la bibliothèque arduino
  TCCR0A=0;
  TCCR0B=3;
  //  TIMSK0=_BV(TOIE0);
  wdt_disable(); 
}

void USB_init(void){
 //  USBDevice;
  USBDevice_attach();
 // Ouverture de la communication USB et attente de connection
  while (! Serial_test()) {
  }; // wait for serial port to connect. 
}

void USB_OUT( unsigned char data){
  Serial_write(data);
}

unsigned char USB_IN(void){
  char c;
  while((c=Serial_read())<0);
  return c ;
}


void USB_writestr(char *s){
  while(*s!='\0')
    USB_OUT(*s++);
}

int USB_readstr(char *s, int n){
  int i=0;char c;
  while(((c=USB_IN())!='\n') && (c!='\r') && (++i<n))
    *s++=c;
  *s='\0';
  return (i);
}
