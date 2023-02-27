/*
 * Biblioth�que de communication USB et UART
 * Auteurr M.Bernier
 * Date : 08/04/2013
*/
#ifndef _LIBTTYCOM_H
#define _LIBTTYCOM_H

typedef unsigned char u8;
#define RXTXMAX 80
enum MODE{RX=1, TX, RXTX};


#define USBMODE
void init_olimexIno(void);
void USB_init(void);
void USB_OUT( unsigned char data);
unsigned char USB_IN(void);
void USB_writestr(char *s);
int USB_readstr(char *s, int n);
/*
* Gestion de la liaison s�rie
*/

#define FREQ(x) (int)(F_CPU/16/x-1)
#define BAUD 9600

/* /\*Routine d'initialisation de la liason s�rie *\/ */
/* void uart_init(int, u8); */

/* /\* Routine de transmission d'une donn�e *\/ */
/* void uart_send( char *); */

/* /\* R�ception d'une donn�e*\/ */
/* unsigned char uart_recv(char *); */


#endif
